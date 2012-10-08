#ifndef BOOST_ASIO_AREX_IPV4_HEADER_HPP
#define BOOST_ASIO_AREX_IPV4_HEADER_HPP

#include <iostream>
#include <cstdint>
#include <string>
#include <netdb.h>
#include <netinet/ip.h>
#include <boost/asio/ip/address.hpp>
#include <arex/protocol_header.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace arex {

/*
struct iphdr
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ihl:4;
    unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int version:4;
    unsigned int ihl:4;
#else
# error	"Please fix <bits/endian.h>"
#endif
    u_int8_t tos;
    u_int16_t tot_len;
    u_int16_t id;
    u_int16_t frag_off;
    u_int8_t ttl;
    u_int8_t protocol;
    u_int16_t check;
    u_int32_t saddr;
    u_int32_t daddr;
    // The options start here.
  };
*/


//
// Protocol Entry of OSI layer 4 (Transport layer)
//
// Exception: If a given entry doesn't exist, throw an exception in ctors
//              or
//            operator= throws an exception if a copy of std::string fails
//
class protocol_entry {
public:

    enum { name_buffer_length = 1024 };
    typedef struct protoent entry_type;
    typedef std::vector<std::string> aliases_list_t;

    protocol_entry(int protocol)
    {
        char name_buf_[name_buffer_length];
        entry_type result_buf_, *result_;
    
        int retval = getprotobynumber_r(
            protocol,
            &result_buf_,
            name_buf_,
            sizeof name_buf_,
            &result_
        );
       
        if ( result_ == nullptr || retval != 0 )
            throw_error();

        store_entry_elem(result_buf_);
    }
    
    protocol_entry(std::string const& proto_name)
    {
        char name_buf_[name_buffer_length];
        entry_type result_buf_, *result_;

        int retval = getprotobyname_r(
            proto_name.c_str(),
            &result_buf_,
            name_buf_,
            sizeof name_buf_,
            &result_
        );
       
        if ( result_ == nullptr || retval != 0 )
            throw_error();
        
        store_entry_elem(result_buf_);
    }

    int number() const
    {
        return protocol_number_;
    }

    std::string const& name() const
    {
        return protocol_name_;
    }

    aliases_list_t const& aliases() const
    {
        return aliases_;
    }

    protocol_entry& operator=(protocol_entry const& rhs)
    {
        protocol_number_ = rhs.protocol_number_;
        protocol_name_   = rhs.protocol_name_;
        aliases_         = rhs.aliases_;
        return *this;
    }

    bool operator==(protocol_entry const& rhs) const
    {
        return (number() == rhs.number());
    }
    
    bool operator!=(protocol_entry const& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator<(protocol_entry const& rhs) const
    {
        return (number() < rhs.number());
    }

    bool operator>(protocol_entry const& rhs) const
    {
        return (number() > rhs.number());
    }

    bool operator<=(protocol_entry const& rhs) const
    {
        return (*this < rhs || *this == rhs);
    }
    
    bool operator>=(protocol_entry const& rhs) const
    {
        return (*this > rhs || *this == rhs);
    }

private:

    static void throw_error()
    {
        throw std::runtime_error(
            "get_proto_entry: no more records or buffer is too small"
        );
    }

    void store_entry_elem(entry_type const& ent)
    {
        protocol_number_ = ent.p_proto;
        protocol_name_   = ent.p_name;
        for ( char **p = ent.p_aliases; *p != nullptr; ++p )
            aliases_.push_back(*p);
    }

    int protocol_number_;
    std::string protocol_name_;
    aliases_list_t aliases_;
};


class ipv4_header : public protocol_header {
public:
    enum { IP_LENGTH_UNIT = 4, IP_DEFAULT_TTL = IPDEFTTL }; 
    typedef struct iphdr header_type;

    ipv4_header() : rep_{0} {}
    explicit ipv4_header(const header_type &iph) : rep_(iph) {}
    ~ipv4_header() {}
    
    uint8_t  version() const { return rep_.version; }
    uint8_t  ihl() const { return rep_.ihl; }
    uint8_t  tos() const { return rep_.tos; }
    uint16_t tot_len() const { return ntohs(rep_.tot_len); }
    uint16_t id() const { return ntohs(rep_.id); }
    uint16_t frag_off() const { return ntohs(rep_.frag_off); }
    uint8_t  ttl() const { return rep_.ttl; }
    uint8_t  protocol() const { return rep_.protocol; }
    uint16_t check() const { return ntohs(rep_.check); }
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
    void ihl(uint8_t ihl) { rep_.ihl = ihl; }
    void tos(uint8_t tos) { rep_.tos = tos; }
    void tot_len(uint16_t tot_len) { rep_.tot_len = htons(tot_len); }
    void id(uint16_t id) { rep_.id = htons(id); }
    void frag_off(uint16_t frag_off) { rep_.frag_off = htons(frag_off); }
    void ttl(uint8_t ttl) { rep_.ttl = ttl; }
    void protocol(uint8_t protocol) { rep_.protocol = protocol; }
    void check(uint16_t check) { rep_.check = htons(check); }
    void check() { check(0); check( checksum(reinterpret_cast<unsigned short*>(&rep_), length()) ); }
    void saddr(uint32_t saddr) { rep_.saddr = htonl(saddr); }
    void daddr(uint32_t daddr) { rep_.daddr = htonl(daddr); }

    int length() const { return sizeof(rep_); }
    char *get_header() { return reinterpret_cast<char*>(&rep_); }
     
private:
   header_type rep_;
}; 


}   // namespace arex
}   // namespace ip
}   // namespace asio
}   // namespace boost


#endif  // BOOST_ASIO_AREX_IPV4_HEADER_HPP
