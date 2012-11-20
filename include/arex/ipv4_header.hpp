#ifndef BOOST_ASIO_AREX_IPV4_HEADER_HPP
#define BOOST_ASIO_AREX_IPV4_HEADER_HPP

#include <iostream>
#include <cstdint>
#include <string>
#include <netdb.h>
#include <netinet/ip.h>
#include <boost/asio/ip/address.hpp>
#include <arex/protocol_header.hpp>
#include <arex/arex_utils.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace arex {


//                                    
//    0                   1                   2                   3   
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |Version|  IHL  |Type of Service|          Total Length         |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |         Identification        |Flags|      Fragment Offset    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  Time to Live |    Protocol   |         Header Checksum       |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                       Source Address                          |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Destination Address                        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Options                    |    Padding    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//            IPv4 Datagram Header From the figure 4 of RFC791
//

// Internal IPv4 header type
// struct iphdr
//   {
// #if __BYTE_ORDER == __LITTLE_ENDIAN
//     unsigned int ihl:4;
//     unsigned int version:4;
// #elif __BYTE_ORDER == __BIG_ENDIAN
//     unsigned int version:4;
//     unsigned int ihl:4;
// #else
// # error	"Please fix <bits/endian.h>"
// #endif
//     u_int8_t tos;
//     u_int16_t tot_len;
//     u_int16_t id;
//     u_int16_t frag_off;
//     u_int8_t ttl;
//     u_int8_t protocol;
//     u_int16_t check;
//     u_int32_t saddr;
//     u_int32_t daddr;
//     // The options start here.
//   };


class ipv4_header : public protocol_header {
public:

    enum { length_unit = 4, default_ttl = IPDEFTTL }; 
    
    typedef struct iphdr header_type;

    ipv4_header() : rep_{0}
    {
    }

    explicit ipv4_header(header_type const& iph) : rep_(iph)
    {
    }
    
    uint8_t  version() const { return rep_.version; }
    uint8_t  header_length() const { return rep_.ihl; }
    uint8_t  type_of_service() const { return rep_.tos; }
    uint16_t total_length() const { return ntohs(rep_.tot_len); }
    uint16_t id() const { return ntohs(rep_.id); }
    uint16_t fragment_offset() const { return ntohs(rep_.frag_off); }
    uint8_t  ttl() const { return rep_.ttl; }
    uint8_t  protocol() const { return rep_.protocol; }
    uint16_t checksum() const { return ntohs(rep_.check); }
    uint32_t saddr() const { return ntohl(rep_.saddr); }
    uint32_t daddr() const { return ntohl(rep_.daddr); }
    
    boost::asio::ip::address_v4 s_address() const
    {
        return boost::asio::ip::address_v4(ntohl(rep_.saddr));
    }
    boost::asio::ip::address_v4 d_address() const
    {
        return boost::asio::ip::address_v4(ntohl(rep_.daddr));
    }
    
    void version(uint8_t version) { rep_.version = version; }
    void header_length(uint8_t ihl) { rep_.ihl = ihl; }
    void type_of_service(uint8_t tos) { rep_.tos = tos; }
    void total_length(uint16_t tot_len) { rep_.tot_len = htons(tot_len); }
    void id(uint16_t id) { rep_.id = htons(id); }
    void fragment_offset(uint16_t frag_off) { rep_.frag_off = htons(frag_off); }
    void ttl(uint8_t ttl) { rep_.ttl = ttl; }
    void protocol(uint8_t protocol) { rep_.protocol = protocol; }
    void checksum(uint16_t check) { rep_.check = htons(check); }
    void checksum() { checksum(0); checksum( arex::checksum(reinterpret_cast<std::uint16_t*>(&rep_), length()) ); }
    void saddr(uint32_t saddr) { rep_.saddr = htonl(saddr); }
    void daddr(uint32_t daddr) { rep_.daddr = htonl(daddr); }

    std::size_t length() const
    {
        return sizeof(rep_);
    }
    
    char *get_header()
    {
        return reinterpret_cast<char*>(&rep_);
    }
    
    char const* get_header() const
    {
        return reinterpret_cast<char const*>(&rep_);
    }
     
private:
   header_type rep_;
}; 


}   // namespace arex
}   // namespace ip
}   // namespace asio
}   // namespace boost


#endif  // BOOST_ASIO_AREX_IPV4_HEADER_HPP
