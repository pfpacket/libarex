#ifndef BOOST_ASIO_AREX_COMMON_HPP
#define BOOST_ASIO_AREX_COMMON_HPP

#include <cstdint>

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

// Boost headers
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>


// For glibc 2.1 (bug)
#ifndef SOL_PACKET
#define SOL_PACKET 263
#endif


#define AREX_BEGIN_NAMESPACE \
        namespace boost {\
        namespace asio {\
        namespace ip {\
        namespace arex {

#define AREX_END_NAMESPACE \
        }\
        }\
        }\
        }

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


#endif  // BOOST_ASIO_AREX_COMMON_HPP

