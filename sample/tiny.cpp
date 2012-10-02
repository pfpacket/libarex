
#include <iostream>
#include <stdexcept>
#include <arex.hpp>

using namespace std;
namespace arex = boost::asio::ip::arex;

int main(int argc, char **argv)
{
    try {
    
        boost::asio::io_service io_service;

        // typedef packet socket type including ethernet layer
        typedef arex::basic_protocol<AF_PACKET, AF_PACKET, SOCK_RAW, arex::htons(ETH_P_ALL)> ext_opt;

        // Create a socket which has raw TCP protocol
        arex::raw_tcp::socket socket(io_service, arex::raw_tcp::v4());

        // Socket Option for asio socket: IP_HDRINCL
        //boost::asio::ip::arex::basic_option<int> iphdrincl(SOL_IP, IP_HDRINCL, 1);
        arex::ip_hdrincl<true> iphdrincl;
        //boost::system::error_code ec;
        
        // Apply IP_HDRINCL socket option
        socket.set_option(iphdrincl);

        // Name-Resolving using arex::raw_tcp
        arex::raw_tcp::resolver resolver(io_service);
        arex::raw_tcp::resolver::query query(arex::raw_tcp::v4(), "www.google.com", "");
        arex::raw_tcp::endpoint destination = *resolver.resolve(query);

    }
    catch( std::exception &e ) {
        cerr << "[-] Exception: " << e.what() << endl;
    }
    return 0;
}
