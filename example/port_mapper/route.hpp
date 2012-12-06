#ifndef PROC_NET_ROUTE_PARSER_HPP
#define PROC_NET_ROUTE_PARSER_HPP

#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

//
//  illegal_address exception class
//
class illegal_address : public std::runtime_error {
public:
    illegal_address(std::string const& what_arg)
        : std::runtime_error(what_arg) {}
};

//
//  Routing reference class
//  Kernel Routing Table from /proc/net/route
//
class rd_routing_table {
public:

    struct route_info__ {
        std::string ifname;
        struct in_addr dest, gateway;
        unsigned int flags;
        int refcnt, use;
        int metric;
        struct in_addr netmask;
        unsigned int mtu, window;
        unsigned int irtt;
    };
    typedef route_info__                route_info;
    typedef std::vector<route_info>     rilist_t;
    typedef rilist_t::iterator          iterator;
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
            [](route_info const& ri){ return ri.dest.s_addr == 0x00000000; }
        );
    }

    const_iterator find(struct in_addr target) const
    {
        auto it = rilist_.begin(), default_rt = get_default_route();
        for (; it != rilist_.end(); ++it) {
            if (it == default_rt) continue;
            if ((target.s_addr & it->netmask.s_addr) == it->dest.s_addr)
                break;
        }
        return (it == rilist_.end()) ? default_rt : it;
    }

    const_iterator find(std::uint32_t target) const
    {
        return this->find(in_addr{target});
    }

    const_iterator find(std::string const& target) const
    {
        struct in_addr tmp;
        if (!inet_aton(target.c_str(), &tmp))
            throw illegal_address(target);
        return this->find(tmp);
    }

    iterator        begin()         { return rilist_.begin(); }
    const_iterator  begin() const   { return rilist_.begin(); }
    iterator        end()           { return rilist_.end(); }
    const_iterator  end()   const   { return rilist_.end(); }

private:
    
    static bool init_proc_rt_stream(std::istream& is)
    {
        std::string buf;
        return std::getline(is, buf);
    }

    static bool read_route_info(std::istream& is, route_info& ri)
    {
        is  >> ri.ifname >> std::hex >> ri.dest.s_addr >> ri.gateway.s_addr
            >> std::dec >> ri.flags >> ri.refcnt >> ri.use >> ri.metric >> std::hex
            >> ri.netmask.s_addr >> std::dec >> ri.mtu >> ri.window >> ri.irtt;
        return is ? true : false;
    }

    static std::string const proc_route;
    rilist_t rilist_;
};
std::string const rd_routing_table::proc_route = "/proc/net/route";

#endif  // PROC_NET_ROUTE_PARSER_HPP
