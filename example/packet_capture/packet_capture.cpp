//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include <iostream>
#include <string>
#include <functional>
#include <stdexcept>
#include <arex.hpp>
#include <boost/asio.hpp>

using namespace std;
using std::placeholders::_1;
using std::placeholders::_2;
namespace arex = boost::asio::ip::arex;

class packet_receiver {
public:

    packet_receiver(boost::asio::io_service& io, std::string const& dev)
        : counter_(0), socket_(io, arex::packet_p_all::v4())
    {
        // The first argument is not used for receiving packets
        arex::packet_p_all::endpoint endpoint("FF:FF:FF:FF:FF:FF", dev);
        // Bind the spicified interface
        socket_.bind(endpoint);
        // promiscuous option
        arex::packet_socket_option opt(
            true,
            // This function can change the underlying option structure
            std::bind(&arex::ps_opt_promisc, _1, endpoint.if_index())
        );
        // Enable promiscuous mode
        socket_.set_option(opt);
        start_receive();
    }

    static void print_ether_header(arex::ethernet_header const& eth)
    {
        cout << "Frame : " << (eth.is_802_3() ? "802.3" : "DIX") << endl;
        cout << "Source: " << eth.source() << endl;
        cout << "Dest  : " <<  eth.dest()  << endl;
        cout << "Type  : " << arex::ether_type_str(eth.eth_type()) << endl;
    }

    static void print_ip_header(arex::ipv4_header const& ip)
    {
        // protocol entry
        arex::protocol_entry proto(ip.protocol());
        cout << "  -- IP header --" << endl;
        cout << "From  : " << ip.s_address() << endl;
        cout << "To    : " << ip.d_address() << endl;
        cout << "Proto : " << proto.name() << dec << endl;
        cout << "Total : " << ip.total_length() << endl;
        cout << "ID    : " << hex << "0x" << ip.id() << dec << endl;
    }

    void recv_handler(
            boost::system::error_code const &ec,
            std::size_t size
        )
    {
        if ( ec )
            cerr << "[-] fatal: " << ec.message() << endl;
        else {
            // Decode an ethernet header from streambuf
            recv_buffer_.commit(size);
            std::istream is(&recv_buffer_);
            arex::ethernet_header ethh;
            is >> ethh;
            
            typedef arex::packet_p_all::endpoint::pkttype pkttype;
            int ptype = endpoint_from_.packet_type();
            // Fillter out the packets which are not to us
            if( ptype != pkttype::to_host )
                goto RECVHDR_NEXT;

            // Print the ethernet frame information
            cout << "---- ethernet frame count=" << ++counter_ << " ----" << endl;
            cout << "Length: " << size << endl;
            print_ether_header(ethh);

            // If this frame contains an IP datagram
            if( ethh.eth_type() == arex::ether_type::ip ) {
                arex::ipv4_header ip;
                is >> ip;
                print_ip_header(ip);
            }
            cout << endl;

RECVHDR_NEXT:
            recv_buffer_.consume(recv_buffer_.size());
            start_receive();
        }
    }

    void start_receive()
    {
        socket_.async_receive_from(
            recv_buffer_.prepare(65536), 
            endpoint_from_,
            std::bind(&packet_receiver::recv_handler, this, _1, _2)
        );
    }

private:
    int counter_;
    arex::packet_p_all::socket socket_;
    arex::packet_p_all::endpoint endpoint_from_;
    boost::asio::streambuf recv_buffer_;
};

int main(int argc, char **argv)
{
    try {
        
        if ( argc < 2 ) {
            cout << "Usage: ./a.out INTERFACE" << endl;
            return EXIT_FAILURE;
        }

        boost::asio::io_service io_service;
        packet_receiver recver(io_service, argv[1]);

        io_service.run();

    }
    catch ( std::exception& e ) {
        cerr << "[-] Exception: " << e.what() << endl;
    }
    return EXIT_SUCCESS;
}

