#ifndef BOOST_ASIO_EXT_PROTOCOL_HPP
#define BOOST_ASIO_EXT_PROTOCOL_HPP

#include <boost/asio/detail/config.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/basic_raw_socket.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/basic_resolver_iterator.hpp>
#include <boost/asio/ip/basic_resolver_query.hpp>
#include <boost/asio/detail/push_options.hpp>
#include <arex/arex_utils.hpp>
#include <sys/socket.h>
#include <arex/common.hpp>
#include <arex/arex_utils.hpp>
#include <arex/sll_endpoint.hpp>
#include <arex/packet_socket_service.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace arex {


template<int Family, int FamilyV6, int Type, int Protocol>
class basic_protocol_raw
{
public:
    
    typedef basic_endpoint<basic_protocol_raw>   endpoint;
    typedef basic_raw_socket<basic_protocol_raw> socket;
    typedef basic_resolver<basic_protocol_raw>   resolver;

    static basic_protocol_raw v4()
    {
        return basic_protocol_raw(Protocol, Family);
    }

    static basic_protocol_raw v6()
    {
        return basic_protocol_raw(Protocol, FamilyV6);
    }

    int family() const
    {
        return family_;
    }
    
    int type() const
    {
        return Type;
    }

    int protocol() const
    {
        return protocol_;
    }

    friend bool operator==(const basic_protocol_raw& p1, const basic_protocol_raw& p2)
    {
        return p1.protocol_ == p2.protocol_ && p1.family_ == p2.family_;
    }

    friend bool operator!=(const basic_protocol_raw& p1, const basic_protocol_raw& p2)
    {
        return p1.protocol_ != p2.protocol_ || p1.family_ != p2.family_;
    }

private:
  
    explicit basic_protocol_raw(int protocol_id, int protocol_family)
        :  protocol_(protocol_id),
           family_(protocol_family)
    {
    }

    int protocol_;
    int family_;
};


//
// Basic protocol for sockaddr_ll (packet socket)
//
template<int Family, int Type, int Protocol>
class basic_protocol_ll
{
public:

    typedef boost::asio::ip::arex::sll_endpoint<basic_protocol_ll> endpoint;
    typedef basic_raw_socket<basic_protocol_ll, packet_socket_service<basic_protocol_ll>> socket;
    // Resolver not supported
    //typedef basic_resolver<basic_protocol_ll> resolver;
    
    static basic_protocol_ll proto()
    {
        return basic_protocol_ll(Protocol, Family);
    }

    // deprecated
    static basic_protocol_ll v4()
    {
        return basic_protocol_ll(Protocol, Family);
    }

    int family() const
    {
        return family_;
    }
    
    int type() const
    {
        return Type;
    }

    int protocol() const
    {
        return protocol_;
    }

    friend bool operator==(const basic_protocol_ll& p1, const basic_protocol_ll& p2)
    {
        return p1.protocol_ == p2.protocol_ && p1.family_ == p2.family_;
    }

    friend bool operator!=(const basic_protocol_ll& p1, const basic_protocol_ll& p2)
    {   
        return p1.protocol_ != p2.protocol_ || p1.family_ != p2.family_;
    }

private:

    explicit basic_protocol_ll(int protocol_id, int protocol_family)
      : protocol_(protocol_id),
        family_(protocol_family)
    {
    }

    int protocol_;
    int family_;
};


//
// pre-typedef well-known protocols
//
// raw socket for ICMP
typedef basic_protocol_raw<AF_INET, AF_INET6, SOCK_RAW, IPPROTO_ICMP> raw_icmp;
// raw socket for TCP
typedef basic_protocol_raw<AF_INET, AF_INET6, SOCK_RAW, IPPROTO_TCP> raw_tcp;
// packet socket including IP layer 
typedef basic_protocol_ll<AF_PACKET, SOCK_DGRAM, arex::htons(ETH_P_IP)> packet_p_ip;
// packet socket including IPv6 layer 
typedef basic_protocol_ll<AF_PACKET, SOCK_DGRAM, arex::htons(ETH_P_IPV6)> packet_p_ipv6;
// packet socket including lowest layer ethernet (lowest layer socket)
typedef basic_protocol_ll<AF_PACKET, SOCK_RAW, arex::htons(ETH_P_ALL)> packet_p_all;


} // namespace arex
} // namespace ip
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif  // BOOST_ASIO_EXT_PROTOCOL_HPP
