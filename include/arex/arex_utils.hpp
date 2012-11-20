#ifndef BOOST_ASIO_AREX_UTILS_HPP
#define BOOST_ASIO_AREX_UTILS_HPP

#include <boost/asio/streambuf.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace arex {


inline namespace network {


constexpr uint16_t htons(uint16_t s)
{
    return (s >> 8) | (s << 8);
}

constexpr uint32_t htonl(uint32_t s)
{
    return (
        ((s & 0x000000FF) << 24) | ((s & 0x0000FF00) << 8)
      | ((s & 0xFF000000) >> 24) | ((s & 0x00FF0000) >> 8)
    );
}

constexpr uint16_t ntohs(uint16_t s)
{
    return arex::htons(s);
}

constexpr uint32_t ntohl(uint32_t s)
{
    return arex::htonl(s);
}

std::uint16_t checksum(std::uint16_t *buf, int bufsz)
{
    unsigned long sum = 0;
    while (bufsz > 1) {
        sum += *buf++;
        bufsz -= 2;
    }
    if (bufsz == 1)
        sum += *(unsigned char *)buf;
    sum = (sum & 0xffff) + (sum >> 16);
    sum = (sum & 0xffff) + (sum >> 16);
    return ~sum;
}


}   // inline namespace network


inline namespace generic {


char const* streambuf_to_ptr(boost::asio::streambuf const& streambuf)
{
    return boost::asio::buffer_cast<char const*>(streambuf.data());
}


}   // inline namespace generic

}   // namespace arex
}   // namespace ip
}   // namespace asio
}   // namespace boost


#endif  // BOOST_ASIO_AREX_UTILS_HPP
