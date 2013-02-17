#ifndef BOOST_ASIO_AREX_MAC_ADDRSS_HPP
#define BOOST_ASIO_AREX_MAC_ADDRSS_HPP


#include <arex/common.hpp>
#include <array>
#include <string>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <stdexcept>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <linux/if_ether.h>


namespace boost {
namespace asio {
namespace ip {
namespace arex {
//namespace ether {


class mac_address {
public:
    enum { addr_length = ETH_ALEN };
    
    typedef struct ether_addr internal_type;
    typedef std::array<uint8_t, addr_length> bytes_type;

    mac_address() : addr_{{0}}
    {
    }
    
    explicit mac_address(internal_type const &addr) : addr_(addr)
    {
    }

    explicit mac_address(bytes_type const &abin) : addr_{{0}}
    {
        std::copy(abin.begin(), abin.end(), &addr_.ether_addr_octet[0]);
    }
    
    explicit mac_address(std::string const &str) : addr_{{0}}
    {
        if (!ether_aton_r(str.c_str(), &addr_))
            throw std::invalid_argument(
                "invalid_argument: could not convert to internal type: " + str
            );
    }

    mac_address &operator=(mac_address const &other)
    {
        addr_ = other.addr_;
        return *this;
    }

    bool operator==(mac_address const &other) const
    {
        for( int i = 0; i < addr_length; ++i )
            if ((addr_.ether_addr_octet[i] != other.addr_.ether_addr_octet[i]))
                return false;
        return true;
    }
    
    bool operator!=(mac_address const &other) const
    {
        return !(*this == other);
    }
    
    static mac_address from_string(char const *str)
    {
        return mac_address(str);
    }
    
    static mac_address from_string(std::string const &str)
    {
        return mac_address(str);
    }
    
    std::string to_string() const
    {
        char ret[18] = {0};
        ether_ntoa_r(&addr_, ret);
        return std::string(ret);
    }
    
    bytes_type to_bytes() const
    {
        bytes_type ret;
        std::copy(&addr_.ether_addr_octet[0],
            &addr_.ether_addr_octet[0] + addr_length, ret.begin());
        return ret;
    }

    internal_type const &get_internal() const
    {
        return addr_;
    }

private:
    internal_type addr_;
};


template<typename Elem, typename Traits>
auto operator<<(std::basic_ostream<Elem, Traits> &os, mac_address const &addr) -> decltype(os)
{
    os << addr.to_string();
    return os;
}


//}   // namespace ether
}   // namespace arex
}   // namespace ip
}   // namespace asio
}   // namespace boost


#endif  // BOOST_ASIO_AREX_MAC_ADDRSS_HPP
