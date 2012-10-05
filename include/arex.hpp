#ifndef BOOST_ASIO_AREX_HPP
#define BOOST_ASIO_AREX_HPP


//
//            - AREX -
//      Asio Rawsocket EXtension
//
//   AREX is a library for 
//          raw socket services 
//                 with powerful Boost.Asio
//
//

//     #
//    # #     ####      #     ####
//   #   #   #          #    #    #
//  #     #   ####      #    #    #
//  ######       #     #    #    #
//  #     #  #    #     #    #    #
//  #     #   ####      #     ####
//
//          ######
//          #     #    ##    #    #   ####    ####    ####   #    #  ######   #####
//          #     #   #  #   #    #  #       #    #  #    #  #   #   #          #
//          ######   #    #  #    #   ####   #    #  #       ####    #####      #
//          #   #    ######  # ## #       #  #    #  #       #  #    #          #
//          #    #   #    #  ##  ##  #    #  #    #  #    #  #   #   #          #
//          #     #  #    #  #    #   ####    ####    ####   #    #  ######     #
//
//                  ####### #     #
//                  #        #   #    #####  ######  #    #  #####   ######  #####
//                  #         # #       #    #       ##   #  #    #  #       #    #
//                  #####      #        #    #####   # #  #  #    #  #####   #    #
//                  #         # #       #    #       #  # #  #    #  #       #    #
//                  #        #   #      #    #       #   ##  #    #  #       #    #
//                  ####### #     #     #    ######  #    #  #####   ######  #####


// Use typedef-integer types in cstdint in arex
#include <cstdint>

#if defined(__linux__)
// POSIX and socket headers
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <signal.h>
#include <wait.h>
#elif defined(_MSC_VER) || defined(_WIN32)
#include <winsock2.h>
#endif

// Boost headers
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>


//
// namespace for arex
//
namespace boost {
namespace asio {
namespace ip {
namespace arex {


}   // namespace arex
}   // namespace ip
}   // namespace asio
}   // namespace boost


//
// #define Parametors
// AREX_DEBUG_MESG - for debug messages
// AREX_ENABLE_PACKET_SOCKET - enable packet socket supports
//
#if defined(AREX_DEFINE_NAMESPACE)
namespace AREX_DEFINE_NAMESPACE = boost::asio::ip::arex;
#endif


// include all arex headers
#include <arex/arex_utils.hpp>
#include <arex/arex_options.hpp>
#include <arex/arex_protocol.hpp>
#include <arex/protocol_header.hpp>
#include <arex/ipv4_header.hpp>
#include <arex/tcp_header.hpp>
#include <arex/mac_address.hpp>
#include <arex/ethernet_header.hpp>
#include <arex/packet_socket_service.hpp>
#include <arex/sll_endpoint.hpp>

#endif  // BOOST_ASIO_AREX_HPP
