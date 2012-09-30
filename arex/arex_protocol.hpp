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
#include <sys/socket.h>

namespace boost {
namespace asio {
namespace ip {
namespace arex {

template<int Family, int FamilyV6, int Type, int Protocol>
class basic_protocol
{
public:
  typedef basic_endpoint<basic_protocol> endpoint;

  static basic_protocol v4()
  {
    return basic_protocol(Protocol, Family);
  }

  static basic_protocol v6()
  {
    return basic_protocol(Protocol, FamilyV6);
  }

  int type() const
  {
    return Type;
  }

  int protocol() const
  {
    return protocol_;
  }

  int family() const
  {
    return family_;
  }

  typedef basic_raw_socket<basic_protocol> socket;

  typedef basic_resolver<basic_protocol> resolver;

  friend bool operator==(const basic_protocol& p1, const basic_protocol& p2)
  {
    return p1.protocol_ == p2.protocol_ && p1.family_ == p2.family_;
  }

  friend bool operator!=(const basic_protocol& p1, const basic_protocol& p2)
  {
    return p1.protocol_ != p2.protocol_ || p1.family_ != p2.family_;
  }

private:
  
  explicit basic_protocol(int protocol_id, int protocol_family)
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
typedef basic_protocol<AF_INET, AF_INET6, SOCK_RAW, IPPROTO_ICMP> raw_icmp;
// raw socket for TCP
typedef basic_protocol<AF_INET, AF_INET6, SOCK_RAW, IPPROTO_TCP> raw_tcp;
// packet socket including IP layer 
typedef basic_protocol<AF_PACKET, AF_PACKET, SOCK_DGRAM, arex::htons(ETH_P_IP)> packet_ip;
// packet socket including lowest layer ethernet (lowest layer socket)
typedef basic_protocol<AF_PACKET, AF_PACKET, SOCK_RAW, arex::htons(ETH_P_ALL)> packet_eth_all;


} // namespace arex
} // namespace ip
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif  // BOOST_ASIO_EXT_PROTOCOL_HPP
