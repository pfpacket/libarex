//
// ip/detail/sll_endpoint.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_AREX_SLL_ENDPOINT_HPP
#define BOOST_ASIO_AREX_SLL_ENDPOINT_HPP

#include <boost/asio/detail/config.hpp>
#include <string>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/detail/winsock_init.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/ip/address.hpp>

#include <boost/asio/detail/push_options.hpp>

// Linux headers
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ether.h>

namespace boost {
namespace asio {
namespace ip {
namespace arex {


namespace detail {
    typedef sockaddr_ll sockaddr_ll_type;
} // namespace detail


// Helper class for implementating an packet socket endpoint.
template<typename InternetProtocol>
class sll_endpoint {
public:

    typedef InternetProtocol protocol_type;
    
    // man 7 packet
    enum pkttype
    {
        to_host     =   PACKET_HOST,
        broadcast   =   PACKET_BROADCAST,
        multicast   =   PACKET_MULTICAST,
        other_host  =   PACKET_OTHERHOST,
        outgoing    =   PACKET_OUTGOING,
        loopback    =   PACKET_LOOPBACK,
        fastroute   =   PACKET_FASTROUTE
    };

    // Default constructor.
    sll_endpoint() : data_{}
    {
        std::memset(&data_, sizeof data_, 0);
        data_.sll.sll_family = AF_PACKET;
        data_.sll.sll_halen= IFHWADDRLEN;
        data_.sll.sll_protocol = protocol_type::v4().protocol();
    }

    sll_endpoint(std::string const& macaddr, std::string const& device_name) : data_{}
    {
        std::memset(&data_, sizeof data_, 0);
        data_.sll.sll_family = AF_PACKET;
        data_.sll.sll_halen = IFHWADDRLEN;
        data_.sll.sll_protocol = protocol_type::proto().protocol();
        data_.sll.sll_ifindex = ::if_nametoindex(device_name.c_str());
        namespace system = boost::system;
        if ( data_.sll.sll_ifindex == 0 )
            boost::asio::detail::throw_error(
               system::error_code(system::errc::no_such_device, system::system_category())
            );
        ether_aton_r(macaddr.c_str(), reinterpret_cast<struct ether_addr *>(&data_.sll.sll_addr));
    }

    // Copy constructor.
    sll_endpoint(const sll_endpoint& other) : data_(other.data_)
    {
    }

    // Assign from another sll_endpoint.
    sll_endpoint& operator=(const sll_endpoint& other)
    {
        data_ = other.data_;
        return *this;
    }

    protocol_type protocol()
    {
        return protocol_type::proto();
    }

    // Get the underlying sll_endpoint in the native type.
    boost::asio::detail::socket_addr_type* data()
    {
        return &data_.base;
    }

    // Get the underlying sll_endpoint in the native type.
    const boost::asio::detail::socket_addr_type* data() const
    {
        return &data_.base;
    }

    // Get the underlying size of the sll_endpoint in the native type.
    std::size_t size() const
    {
        return sizeof(boost::asio::ip::arex::detail::sockaddr_ll_type);
    }

    void resize(std::size_t new_size)
    {
        if (new_size > sizeof(boost::asio::detail::sockaddr_storage_type))
        {
            boost::system::error_code ec(boost::asio::error::invalid_argument);
            boost::asio::detail::throw_error(ec);
        }
    }

    // Get the capacity of the sll_endpoint in the native type.
    std::size_t capacity() const
    {
        return sizeof(data_);
    }

    // Get underlying packet type (Available after receiving)
    int packet_type() const
    {
        return data_.sll.sll_pkttype;
    }

    int protocol_number() const
    {
        return data_.sll.sll_protocol;
    }

    int family() const
    {
        return data_.sll.sll_family;
    }

    int if_index() const
    {
        return data_.sll.sll_ifindex;
    }


private:
    // The underlying packet socket address.
    union data_union
    {
        boost::asio::detail::socket_addr_type base;
        boost::asio::ip::arex::detail::sockaddr_ll_type sll;
    } data_;
};


} // namespace arex
} // namespace ip
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_IP_DETAIL_ENDPOINT_HPP
