#ifndef BOOST_ASIO_AREX_ETHER_HEADER_HPP
#define BOOST_ASIO_AREX_ETHER_HEADER_HPP

#include <arex/common.hpp>
#include <map>
#include <string>
#include <sstream>
#include <cstdint>
#include <algorithm>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <linux/if_ether.h>
#include <arex/mac_address.hpp>
#include <arex/protocol_header.hpp>
#include <arex/arex_utils.hpp>


namespace boost {
namespace asio {
namespace ip {
namespace arex {


enum ether_type
{ 
    xerox_pup     =   ETHERTYPE_PUP,
    sprite        =   ETHERTYPE_SPRITE,
    ip            =   ETHERTYPE_IP,
    ipv6          =   ETHERTYPE_IPV6,
    arp           =   ETHERTYPE_ARP,
    reverse_arp   =   ETHERTYPE_REVARP,
    appletalk     =   ETHERTYPE_AT,
    appletalk_arp =   ETHERTYPE_AARP,
    vlan          =   ETHERTYPE_VLAN,
    ipx           =   ETHERTYPE_IPX, 
    loopback      =   ETHERTYPE_LOOPBACK
};


class ethernet_header : public protocol_header
{
private:

    struct etherhdr {
        mac_address::internal_type h_dest;
        mac_address::internal_type h_source;
        uint16_t ether_type;
    } __attribute__((packed));

    etherhdr rep_;

public:

    enum { header_length = sizeof rep_ };

    typedef etherhdr header_type;

    ethernet_header() : rep_{{{0}}, {{0}}, 0}
    {
    }

    ethernet_header(ethernet_header const &header) 
        : rep_(header.rep_)
    {
    }

    void source(mac_address const &source)
    {
        rep_.h_source = source.get_internal();
    }

    void dest(mac_address const &dest)
    {
        rep_.h_dest = dest.get_internal();
    }

    void type(uint16_t type)
    {
        rep_.ether_type = arex::htons(type);
    }
    
    void eth_type(ether_type etype)
    {
        type(static_cast<uint16_t>(etype));
    }

    mac_address source() const
    {
        return mac_address(rep_.h_source);
    }
 
    mac_address dest() const
    {
        return mac_address(rep_.h_dest);
    }

    uint16_t type() const
    {
        return arex::ntohs(rep_.ether_type);
    }
    
    ether_type eth_type() const
    {
        return static_cast<ether_type>(type());
    }

    // Return if ethernet frame is DIX
    bool is_dix() const
    {
        return (type() >= 1501);
    }
    
    // Return if ethernet frame is 802.3
    bool is_802_3() const
    {
        return (type() < 1501);
    }
    
    std::size_t length() const
    {
        return header_length;
    }

    char *get_header()
    {
        return reinterpret_cast<char*>(&rep_);
    }
    
    char const* get_header() const
    {
        return reinterpret_cast<char const*>(&rep_);
    }

};

std::string ether_type_str(ether_type type)
{
    static std::map<ether_type, std::string> 
     const ether_id_to_str = {
        {ether_type::xerox_pup,     "Xerox PUP"},
        {ether_type::sprite,        "Sprite"},
        {ether_type::ip,            "IPv4"},
        {ether_type::ipv6,          "IPv6"},
        {ether_type::arp,           "ARP"},
        {ether_type::reverse_arp,   "Reverse ARP"},
        {ether_type::appletalk,     "AppleTalk"},
        {ether_type::appletalk_arp, "AppleTalk ARP"},
        {ether_type::vlan,          "VLAN"},
        {ether_type::ipx,           "IPX"},
        {ether_type::loopback,      "Loopback"}
    };
    std::ostringstream ss;
    try {
        ss << ether_id_to_str.at(type);
    }
    catch (...) {
        ss << "0x" << std::hex << static_cast<std::uint16_t>(type);
    }
    return ss.str();
}


}   // namespace arex
}   // namespace ip
}   // namespace asio
}   // namespace boost

#endif  // BOOST_ASIO_AREX_ETHER_HEADER_HPP
