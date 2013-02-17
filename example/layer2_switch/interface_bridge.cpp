
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <functional>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <arex.hpp>

using std::placeholders::_1;
using std::placeholders::_2;
namespace arex = boost::asio::ip::arex;

class interface_bridge {
public:

    enum netif_t { NIC1, NIC2 };
    typedef boost::asio::streambuf buffer_type;
    typedef std::function<void(boost::system::error_code const&,int)> handler_type;

    interface_bridge(
        boost::asio::io_service& ios,
        std::string const& nic1, std::string const& nic2) 
        :   counter_(0),
            socket1_(ios, arex::packet_p_all::v4()),
            socket2_(ios, arex::packet_p_all::v4()),
            start1_(std::bind(&interface_bridge::recv_handler, this, _1, _2, NIC1)),
            start2_(std::bind(&interface_bridge::recv_handler, this, _1, _2, NIC2))
    {
        init_mapper(nic1, nic2);
        setup_bridge();
        start_receive(NIC1);
        start_receive(NIC2);
    }

private:

    void init_mapper(std::string const&, std::string const&);
    void setup_bridge();
    void start_receive(netif_t);
    void recv_handler(boost::system::error_code const&, int, netif_t);

    static void make_promisc(arex::packet_p_all::socket&, int);
    static arex::mac_address ifhw_address(std::string const&);
    static void print_ether_header(arex::ethernet_header const&);

    std::uint64_t counter_;
    arex::packet_p_all::socket socket1_, socket2_;
    arex::packet_p_all::endpoint sender1_, sender2_;
    buffer_type buffer1_, buffer2_;
    handler_type start1_, start2_;

    // Interface mappers
    std::map<netif_t, netif_t> ifdest_; // destination interface
    std::map<netif_t, std::string> ifname_; // interfaces' name
    std::map<netif_t, std::reference_wrapper<arex::packet_p_all::socket>> ifsoc_;  // sockets
    std::map<netif_t, std::reference_wrapper<arex::packet_p_all::endpoint>> ifep_; // endpoint
    std::map<netif_t, std::reference_wrapper<buffer_type>> ifbuf_;
    std::map<netif_t, handler_type> ifhdl_;
};

void interface_bridge::init_mapper(std::string const& nic1, std::string const& nic2)
{
    ifdest_[NIC1] = NIC2;  ifdest_[NIC2] = NIC1;
    ifname_[NIC1] = nic1;  ifname_[NIC2] = nic2;
    ifsoc_.insert(std::make_pair(NIC1, std::ref(socket1_)));
    ifsoc_.insert(std::make_pair(NIC2, std::ref(socket2_)));
    ifep_ .insert(std::make_pair(NIC1, std::ref(sender1_)));
    ifep_ .insert(std::make_pair(NIC2, std::ref(sender2_)));
    ifbuf_.insert(std::make_pair(NIC1, std::ref(buffer1_)));
    ifbuf_.insert(std::make_pair(NIC2, std::ref(buffer2_)));
    ifhdl_.insert(std::make_pair(NIC1, std::ref(start1_)));
    ifhdl_.insert(std::make_pair(NIC2, std::ref(start2_)));
}

void interface_bridge::setup_bridge()
{
    arex::packet_p_all::endpoint ep1("FF:FF:FF:FF:FF:FF", ifname_[NIC1]);
    arex::packet_p_all::endpoint ep2("FF:FF:FF:FF:FF:FF", ifname_[NIC2]);
    socket1_.bind(ep1);
    socket2_.bind(ep2);
    make_promisc(socket1_, ep1.if_index());
    make_promisc(socket2_, ep2.if_index());
}

void interface_bridge::start_receive(netif_t ifnum)
{
    buffer_type& buf = ifbuf_.at(ifnum);
    buf.consume(buf.size());
    ifsoc_.at(ifnum).get().async_receive_from(
        buf.prepare(65535),
        ifep_.at(ifnum),
        ifhdl_.at(ifnum)
    );
}

void interface_bridge::recv_handler(
    boost::system::error_code const& err,
    int size, netif_t ifnum)
{
    if (err)
        std::cout << "[-] " << ifname_[ifnum] << ": Fatal: " << err.message() << std::endl;
    else { do {
        buffer_type& buffer = ifbuf_.at(ifnum);
        buffer.commit(size);
        
        // fillter out packets
        typedef arex::packet_p_all::endpoint::pkttype pkttype;
        int pkt_type = ifep_.at(ifnum).get().packet_type();
        if ((1522 < size || size < 32) || (pkt_type == pkttype::outgoing))
            break;

        arex::ethernet_header ethh;
        arex::streambuf_to_header(ethh, buffer);
        std::cout << boost::format("No.%-9d %-" +
                std::to_string(std::max(ifname_[NIC1].length(), ifname_[NIC2].length()))
                + "s  %4d bytes  ") % ++counter_ % ifname_[ifnum] % size;
        print_ether_header(ethh);

        ifsoc_.at(ifdest_[ifnum]).get().send_to(
            buffer.data(),
            arex::packet_p_all::endpoint("0:0:0:0:0:0", ifname_[ifdest_[ifnum]])
        );
    } while (false); }
    start_receive(ifnum);
}

void interface_bridge::make_promisc(arex::packet_p_all::socket& socket, int ifnumber)
{
    arex::packet_socket_option opt(
        true,
        std::bind(&arex::ps_opt_promisc, _1, ifnumber)
    );
    socket.set_option(opt);
}

void interface_bridge::print_ether_header(arex::ethernet_header const& eth)
{
    std::cout << boost::format("%17s --> %-17s type=%s\n")
        % eth.source() % eth.dest() % arex::ether_type_str(eth.eth_type());
}

int main(int argc, char **argv)
{
    int exit_code = EXIT_SUCCESS;
    try {
        if (argc < 3)
            throw std::runtime_error(
                "Too few arguments\n"
                "Usage: ./bridge NIC1 NIC2"
            );

        boost::asio::io_service io_service;
        interface_bridge bridge(io_service, argv[1], argv[2]);
        io_service.run();
    }
    catch (std::exception &e) {
        std::cerr << "[-] Exception: " << e.what() << std::endl;
        exit_code = EXIT_FAILURE;
    }
    return exit_code;
}
