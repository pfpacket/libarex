#ifndef BOOST_ASIO_EXT_OPTIONS_HPP_
#define BOOST_ASIO_EXT_OPTIONS_HPP_

#include <netinet/in.h>
#include <sys/socket.h>

namespace boost {
namespace asio {
namespace ip {
namespace arex {


// This meets SettableSocketOption requirements
// See the documents of Boost.Asio for more information
template<typename ValueType = int>
class basic_option {
public:

    basic_option() = default;
    basic_option(int level, int name, ValueType const &ov) 
        :   level_(level),
            name_(name),
            size_(sizeof ov),
            optval_(ov)
        {}
    basic_option(int level, int name, ValueType const &ov, size_t size) 
        :   level_(level),
            name_(name),
            size_(size),
            optval_(ov)
        {}

    template<typename Protocol>
    int level(Protocol const &p) const
    {
        return level_;
    }

    template<typename Protocol>
    int name(Protocol const &p) const
    {
        return name_;
    }

    template<typename Protocol>
    void const *data(Protocol const &p) const 
    {
        // Should return a pointer that is convertible to void*
        return reinterpret_cast<void const *>(&optval_);
    }

    template<typename Protocol>
    size_t size(Protocol const &p) const
    {
        // size() returns the size of *data()
        return size_;
    }
     
private:
    int const level_, name_, size_;
    ValueType const optval_;
};


template<int Level, int Name, bool Init = true>
class binary_option {
public:
    binary_option() = default;
    binary_option(bool ov) : optval(ov ? 1 : 0) {}
    ~binary_option() = default;

    template<typename Protocol>
    int level(Protocol const &p) const
    {
        return Level;
    }

    template<typename Protocol>
    int name(Protocol const &p) const
    {
        return Name;
    }

    template<typename Protocol>
    void const *data(Protocol const &p) const
    {
        return reinterpret_cast<void const *>(&optval);
    }

    template<typename Protocol>
    int size(Protocol const &p) const
    {
        return sizeof(optval);
    }
     
private:
    int optval = Init;
};


// Binary option for IP_HDRINCL
template<bool Init>
using ip_hdrincl = binary_option<SOL_IP, IP_HDRINCL, Init>;
// Above is equal to arex::basic_option<int> iphdrincl(SOL_IP, IP_HDRINCL, 1);


} // namespace arex
} // namespace ip
} // namespace asio
} // namespace boost

#endif  // BOOST_ASIO_EXT_OPTIONS_HPP_
