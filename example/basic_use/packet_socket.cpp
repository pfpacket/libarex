//
//    Sample Program using libarex
//  
//  [ create and use the packet socket with Boost.Asio ]
//
//
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include <iostream>
#include <functional>
#include <stdexcept>
#include <arex.hpp>
#include <boost/asio.hpp>

using namespace std;
using std::placeholders::_1;
using std::placeholders::_2;
namespace arex = boost::asio::ip::arex;

void print_handler(boost::system::error_code const &ec, size_t size)
{
    if ( ec )
        cout << "[-] Error occurred: " << ec.message() << endl;
    else
        cout << "[*] print_handler: byte_tranferred = " << size << " bytes" << endl;
}

void ip_handler(boost::system::error_code const &ec, size_t size,
        boost::asio::streambuf &recv_buffer, arex::packet_p_ip::endpoint &endpoint)
{
    if( ec )
        cout << "[-] Error occurred: " << ec.message() << endl;
    else {
        recv_buffer.commit(size);
        std::istream is(&recv_buffer);
        arex::ipv4_header ip;
        is >> ip;
        cout << "[*] ip_handler: Received packet:" << endl;
        cout << "IP version: " << static_cast<int>(ip.version()) << endl;
        cout << "Protocol: " << static_cast<int>(ip.protocol()) << endl;
        cout << "Header length: " << ip.header_length() * ip.length_unit << endl;
        cout << "Total length: " << ip.total_length() << endl;
        cout << "From: " << ip.address_to_string(ip.saddr()) << endl;
        cout << "To: " << ip.address_to_string(ip.daddr()) << endl;
    }
}

int main(int argc, char **argv)
{
    try {
    
        // io_service
        boost::asio::io_service io_service;

        // Create a socket which has lowest layer access to IP (packet socket)
        arex::packet_p_ip::socket socket(io_service, arex::packet_p_ip::v4());
        
        // An endpoint for packet socket
        // endpoint requires destination MAC address and a device name to send to
        arex::packet_p_ip::endpoint endpoint("FF:FF:FF:FF:FF:FF", "wlan0");

        // Bind the network interface
        // Now this socket can receive IP datagrams only from the specified interface
        socket.bind(endpoint);

        // Dummy buffer to send to
        char buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        // Asynchronous `send` operation 
        cout << "[*] async_send_to ..." << endl;
        socket.async_send_to(
                boost::asio::buffer(buffer, sizeof buffer),
                endpoint,
                &print_handler
            );
        
        // Asynchronous `recv` operation
        cout << "[*] async_recv_from ..." << endl;
        arex::packet_p_ip::endpoint endpoint_from;
        boost::asio::streambuf recv_buffer;
        socket.async_receive_from(
                recv_buffer.prepare(65536),
                endpoint_from,
                std::bind(&ip_handler, _1, _2, std::ref(recv_buffer), endpoint_from)
            );

        io_service.run();
    }
    catch ( std::exception &e ) {
        cerr << "[-] Exception: " << e.what() << endl;
    }
    return 0;
}

