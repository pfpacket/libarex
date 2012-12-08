#ifndef PROC_NET_ROUTE_PARSER_HPP
#define PROC_NET_ROUTE_PARSER_HPP

#include <arex/common.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include <arex/arex_utils.hpp>

#ifdef  PROC_ROUTE_PARSER_HEADER_ONLY
#warning "May cause compile error like: no such file (boost/../libs/... /error_code.cpp)"
#ifndef BOOST_ASIO_HEADER_ONLY
#   define  BOOST_ASIO_HEADER_ONLY              1
#   define  ROUTE_ASIO_NOT_HEADER_ONLY          1
#endif
#if !defined(BOOST_ERROR_CODE_HEADER_ONLY) && !defined(BOOST_SYSTEM_NO_LIB)
#   define  BOOST_ERROR_CODE_HEADER_ONLY        1
#   define  BOOST_SYSTEM_NO_LIB                 1
#   define  ROUTE_NOT_ERROR_CODE_HEADER_ONLY    1
#endif
#endif  // PROC_ROUTE_PARSER_HEADER_ONLY
#include <boost/asio/ip/address_v4.hpp>
#ifdef  ROUTE_ASIO_NOT_HEADER_ONLY
#   undef   BOOST_ASIO_HEADER_ONLY
#endif
#ifdef  ROUTE_NOT_ERROR_CODE_HEADER_ONLY
#   undef   BOOST_ERROR_CODE_HEADER_ONLY
#   undef   BOOST_SYSTEM_NO_LIB
#endif

AREX_BEGIN_NAMESPACE

//
//  illegal_address exception class
//
class illegal_address : public std::runtime_error {
public:
    illegal_address(std::string const& what_arg)
        : std::runtime_error(what_arg) {}
};

//
//  Read only routing class
//  Referencing Kernel Routing Table from /proc/net/route
//
class rd_routing_table {
public:
    
    typedef boost::asio::ip::address_v4 address_v4;
    struct route_info__ {
        std::string ifname;
        address_v4 dest, gateway, netmask;
        int refcnt, use, metric;
        unsigned int flags, mtu, window, irtt;
    };
    typedef struct route_info__         route_info;
    typedef std::vector<route_info>     rilist_t;
    typedef rilist_t::size_type         size_type;
    typedef rilist_t::const_reference   const_reference;
    typedef rilist_t::const_iterator    const_iterator;

    rd_routing_table()
    {
        std::ifstream proc_rt(proc_route);
        init_proc_rt_stream(proc_rt);
        for (route_info ri; read_route_info(proc_rt, ri); )
            rilist_.push_back(ri);
    }

    const_iterator get_default_route() const
    {
        return std::find_if(rilist_.begin(), rilist_.end(),
            [](route_info const& ri){ return ri.dest.to_ulong() == 0x00000000; }
        );
    }
    
    const_iterator find(std::string const& target) const
    {
        boost::system::error_code ec;
        auto address = address_v4::from_string(target, ec);
        if (ec)
            throw illegal_address(ec.message() + ": " + target);
        return this->find(address.to_ulong());
    }

    const_iterator find(address_v4 const& target) const
    {
        return this->find(target.to_ulong());
    }

    const_iterator find(std::uint32_t target) const
    {
        auto it = rilist_.begin(), default_rt = get_default_route();
        for (; it != rilist_.end(); ++it) {
            if (it == default_rt) continue;
            if ((target & it->netmask.to_ulong()) == it->dest.to_ulong())
                break;
        }
        return (it == rilist_.end()) ? default_rt : it;
    }

    size_type       size()          const   { return rilist_.size(); }
    const_reference at(size_type n) const   { return rilist_.at(n); }
    const_iterator  begin()         const   { return rilist_.begin(); }
    const_iterator  end()           const   { return rilist_.end(); }

private:

    template<typename CharT, typename Traits>
    auto init_proc_rt_stream(std::basic_istream<CharT, Traits>& is) -> decltype(is)
    {
        std::string buf;
        return std::getline(is, buf);
    }

    template<typename CharT, typename Traits>
    auto read_route_info(std::basic_istream<CharT, Traits>& is, route_info& ri) -> decltype(is)
    {
        std::uint32_t dest, gateway, netmask;
        is  >> ri.ifname >> std::hex >> dest >> gateway >> std::dec
            >> ri.flags >> ri.refcnt >> ri.use >> ri.metric >> std::hex
            >> netmask >> std::dec >> ri.mtu >> ri.window >> ri.irtt;
        ri.dest     =   address_v4(arex::ntohl(dest));
        ri.gateway  =   address_v4(arex::ntohl(gateway));
        ri.netmask  =   address_v4(arex::ntohl(netmask));
        return is;
    }

    static std::string const proc_route;
    rilist_t rilist_;
};
std::string const rd_routing_table::proc_route = "/proc/net/route";

AREX_END_NAMESPACE

#endif  // PROC_NET_ROUTE_PARSER_HPP
