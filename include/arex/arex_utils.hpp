#ifndef BOOST_ASIO_AREX_UTILS_HPP
#define BOOST_ASIO_AREX_UTILS_HPP


namespace boost {
namespace asio {
namespace ip {
namespace arex {


// Compile-Time htons (for template parameters)
constexpr uint16_t htons(uint16_t s)
{
    return (s >> 8) | (s << 8);
}

constexpr uint32_t htonl(uint32_t s)
{
    return (
        ((s & 0x000000FF) << 24) | ((s & 0x0000FF00) << 8) | ((s & 0xFF000000) >> 24) | ((s & 0x00FF0000) >> 8)
    );
}


}   // namespace arex
}   // namespace ip
}   // namespace asio
}   // namespace boost


#endif  // BOOST_ASIO_AREX_UTILS_HPP
