#ifndef BOOST_ASIO_AREX_UTILS_HPP
#define BOOST_ASIO_AREX_UTILS_HPP


namespace boost {
namespace asio {
namespace ip {
namespace arex {


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
    return htons(s);
}

constexpr uint32_t ntohl(uint32_t s)
{
    return htonl(s);
}


}   // namespace arex
}   // namespace ip
}   // namespace asio
}   // namespace boost


#endif  // BOOST_ASIO_AREX_UTILS_HPP
