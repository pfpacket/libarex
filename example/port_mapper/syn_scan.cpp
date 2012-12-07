
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <tuple>
#include <thread>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <cstdint>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <arex.hpp>
#include <arex/route.hpp>

using std::placeholders::_1;
using std::placeholders::_2;
using boost::system::error_code;
namespace asio   = boost::asio;
namespace arex   = boost::asio::ip::arex;
namespace chrono = std::chrono;

std::uint32_t get_ifaddr(std::string const& ifname)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    return arex::ntohl(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);
}

class syn_port_mapper {
    struct scan_info {
        int port;
        chrono::steady_clock::time_point sent_time;
        int seq_number, own_port;
    };
public:

    typedef asio::streambuf buffer_type;
    typedef std::shared_ptr<buffer_type> shared_buf_t;
    typedef asio::basic_waitable_timer<chrono::steady_clock> timer_type;
    typedef std::shared_ptr<timer_type> shared_timer;
    typedef boost::asio::ip::arex::binary_option<SOL_IP, IP_HDRINCL, true> iphdr_include;
    enum { default_timeout = 3000, buffer_size = 1550 };
    // aborted means the port wasn't checked
    enum port_state { open, closed, filtered, aborted };

    syn_port_mapper(boost::asio::io_service& io, std::string const& host, int milisec)
        :   timeout_millisec_(milisec),
            io_service_(io), socket_(io, arex::raw_tcp::v4())
    {
        std::srand(std::time(nullptr));
        // name resolving
        arex::raw_tcp::resolver resolver(io);
        arex::raw_tcp::resolver::query query(arex::raw_tcp::v4(), host, "");
        destination_ = *resolver.resolve(query);
        socket_.set_option(iphdr_include(true));
    }

    // start an async port scan
    void start_scan(int portn)
    {
        auto request_buf = std::make_shared<buffer_type>();
        set_syn_segment(*request_buf, portn);
        auto send_time = chrono::steady_clock::now();
        socket_.async_send_to(
            request_buf->data(), destination_,
            std::bind(&syn_port_mapper::scan_handler, this, _1, _2, 
                scan_info{portn, send_time}, request_buf
            )
        );
    }
    
    std::map<int, port_state> const& get_port_map() const
    {
        return port_map_;
    }
    
private:

    void start_receive(scan_info const& info, shared_timer timer)
    {
        auto&& recvbuf = std::make_shared<buffer_type>();
        socket_.async_receive_from(recvbuf->prepare(buffer_size), sender_,
            std::bind(&syn_port_mapper::recv_handler, this, _1, _2, info, recvbuf, timer)
        );
    }
    
    void start_timer(int millisec, scan_info const& info, shared_timer timer)
    {
        timer->expires_from_now(chrono::milliseconds(millisec));
        timer->async_wait(std::bind(&syn_port_mapper::on_timeout, this, _1, info, timer));
    }

    void scan_handler(error_code const& ec, int bytes, scan_info info, shared_buf_t sendbuf)
    {
        if (ec)
            std::cerr << "[-] Scan Error: " << ec.message() << std::endl;
        else {
            auto timer = std::make_shared<timer_type>(io_service_);
            start_timer(timeout_millisec_, info, timer);
            start_receive(info, timer);
        }
    }

    void recv_handler(error_code const& ec, int bytes, scan_info info, shared_buf_t recvbuf, shared_timer timer)
    {
        // this handler can be canceled only by on_timeout handler
        if (ec == asio::error::operation_aborted) {
            if (timeout_port_.find(info.port) == timeout_port_.end())
                start_receive(info, timer);
            else
                // target port seems not to be open
                // and remote host didn't reply rst,ack segment
                // this behavior shows that this port is filtered
                write_map_if_key_not_exists(info.port, port_state::filtered);
            return;
        }
        else if (ec) {
            std::cerr << "[-] Recv Error: " << ec.message() << " on port " << info.port << std::endl;
            write_map_if_key_not_exists(info.port, port_state::aborted);
        }
        else {
            // analyze a received packet ...
            // arex::raw_tcp guarantees that all received packets are TCP segments
            arex::ipv4_header ip;
            arex::tcp_header tcp;

            recvbuf->commit(bytes);
            std::istream is(&(*recvbuf));
            is >> ip >> tcp;    // decode a packet

            if (tcp.syn() && tcp.ack())
                // target port is open
                port_map_[tcp.source()] = port_state::open;
            else if (tcp.rst() && tcp.ack())
                // target port is surely closed
                port_map_[tcp.source()] = port_state::closed;
            else {
                start_receive(info, timer);
                return;
            }
            timer->cancel();
        }
    }

    void on_timeout(error_code const& ec, scan_info info, shared_timer timer)
    {
        // wait operation error occurred
        if (ec == asio::error::operation_aborted);
        else if (ec)
                std::cerr << "[-] Error: timer handler: " << ec.message() << std::endl;
        else {
            // async receive operation timed out
            timeout_port_.insert(info.port);
            socket_.cancel();
        }
    }
    
    void write_map_if_key_not_exists(int key_port, port_state ps)
    {
        if (port_map_.find(key_port) == port_map_.end())
            port_map_[key_port] = ps;
    }

    std::tuple<int, int> set_syn_segment(buffer_type& sbuffer, int port);

    int timeout_millisec_;
    std::set<int> timeout_port_;
    boost::asio::io_service &io_service_;
    arex::raw_tcp::socket socket_;
    arex::raw_tcp::endpoint destination_, sender_;
    std::map<int, port_state> port_map_;
    arex::rd_routing_table rt_;   // kernel IP routing table
};

std::tuple<int, int>    // random_source_port, random_seq
syn_port_mapper::set_syn_segment(buffer_type& sbuffer, int port)
{
    sbuffer.consume(sbuffer.size());
    std::ostream os(&sbuffer);
    
    arex::ipv4_header iphdr;
    iphdr.version(4);
    iphdr.header_length(iphdr.length() / iphdr.length_unit);
    iphdr.type_of_service(0x10);
    iphdr.fragment_offset(IP_DF);
    iphdr.ttl(iphdr.default_ttl);
    iphdr.protocol(IPPROTO_TCP);
    std::uint32_t daddr = destination_.address().to_v4().to_ulong();
    // Select the IP path referencing kernel IP routing table
    iphdr.saddr(get_ifaddr(rt_.find(daddr)->ifname));
    iphdr.daddr(daddr);

    std::uint16_t source = rand();
    std::uint32_t  seq   = rand();
    arex::tcp_header tcp_syn_header;
    tcp_syn_header.source(source);
    tcp_syn_header.dest(port);
    tcp_syn_header.seq(seq);
    tcp_syn_header.doff(20/4);
    tcp_syn_header.syn(true);
    tcp_syn_header.window(tcp_syn_header.default_window_value);
    tcp_syn_header.compute_checksum(iphdr.saddr(), iphdr.daddr());
     
    iphdr.total_length(iphdr.length() + tcp_syn_header.length());
    iphdr.checksum();
    if (!(os << iphdr << tcp_syn_header))
        throw std::runtime_error("cannot write packet to streambuf");
    return std::make_tuple(source, seq);
}

int main(int argc, char **argv)
{
    int exit_code = EXIT_SUCCESS;
    try {
        if (argc < 3)
            throw std::runtime_error(
                "Too few arguments\n"
                "Usage: ./syn_scan HOST PORT_RANGE [TIMEOUT(millisec)]\n"
                "PORT_RANGE should be two numbers delimited by \'-\'"
            );

        // parse port range command line
        std::string port_range(argv[2]);
        std::string::size_type delim_pos = port_range.find('-');
        if (delim_pos == std::string::npos || delim_pos >= port_range.length() - 1)
            throw std::runtime_error("Incorrect port range");
        int const begin_port = boost::lexical_cast<int>(port_range.substr(0, delim_pos));
        int const end_port   = boost::lexical_cast<int>(port_range.substr(delim_pos + 1));
        if (begin_port < 1 || begin_port > 65535 || end_port < 1 || end_port > 65535)
            throw std::runtime_error("Invalid port number specified as a port range");

        // Scanning
        boost::asio::io_service io_service;
        syn_port_mapper scanner(io_service, argv[1],
                argc >= 4 ? boost::lexical_cast<int>(argv[3]) : syn_port_mapper::default_timeout
            );
        for (int port = begin_port; port <= end_port; ++port)
            scanner.start_scan(port);
        std::cout << "[*] Now starting SYN port scan to " << argv[1] << ':' << port_range << std::endl;
        io_service.run();

        // Print the result of scan
        std::cout << "PORT\tSTATE" << std::endl;
        for (auto pair : scanner.get_port_map()) {
            typedef syn_port_mapper::port_state pstate;
            static std::map<pstate, std::string> const pstr = { 
                {pstate::open,     "open"},     {pstate::closed,  "closed"}, 
                {pstate::filtered, "filtered"}, {pstate::aborted, "aborted"}
            };
            std::cout << pair.first << '\t' << pstr.at(pair.second) << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << "[-] Exception: " << e.what() << std::endl;
        exit_code = EXIT_FAILURE;
    }
    return exit_code;
}

