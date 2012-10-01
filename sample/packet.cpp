//
//    Sample Program using libarex
//  
//  [ create and use the packet socket with Boost.Asio ]
//
//
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include <istream>
#include <ostream>
#include <iostream>
#include <stdexcept>
#include <arex/arex.hpp>
#include <boost/asio.hpp>

using namespace std;
namespace arex = boost::asio::ip::arex;

// async operation handler 
//      that just prints whether operation failed or not
void print_handler(boost::system::error_code const &ec, size_t const size)
{
    if ( ec )
        cout << "[-] Error occurred: " << ec.message() << endl;
    else
        cout << "[*] Async operation successfully finished: " << size << " bytes" << endl;
}

int main(int argc, char **argv)
{
    try {
    
        // io_service
        boost::asio::io_service io_service;

        // Create a socket which has lowest layer access to IP
        arex::packet_p_ip::socket socket(io_service, arex::packet_p_ip::v4());
        
        // An endpoint for packet socket
        // endpoint requires destination MAC address and a device name to send to
        arex::packet_p_ip::endpoint endpoint("FF:FF:FF:FF:FF:FF", "lo");

        // Bind the network interface
        // This socket can receive the IP datagram only from specified interface
        socket.bind(endpoint);

        // Dummy buffer to be sent
        char buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        // Asynchronous `send` operation 
        cout << "[*] async_send_to ..." << endl;
        socket.async_send_to(boost::asio::buffer(buffer, sizeof buffer), endpoint, &print_handler);
        
        io_service.run();

/*        // Asynchronous `recv` operation
        cout << "[*] async_recv_from ..." << endl;
        arex::packet_p_ip::endpoint endpoint_from;
        boost::asio::streambuf recv_buffer;
        socket.receive_from(recv_buffer.prepare(65536), endpoint_from);
        
        cout << "[*] Received packet:" << endl;
        std::istream is(&recv_buffer);
        arex::ip_header ip;
        is >> ip;
        cout << "version: " << ip.version() << endl;
*/
    
    }
    catch ( std::exception &e ) {
        cerr << "[-] Exception: " << e.what() << endl;
    }
    return 0;
}

