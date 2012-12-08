#ifndef BOOST_ASIO_EXT_OPTIONS_HPP_
#define BOOST_ASIO_EXT_OPTIONS_HPP_

#include <arex/common.hpp>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <boost/throw_exception.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace arex {

//
//
// These socket options meet SettableSocketOption requirements
// See the documents of Boost.Asio for more information
// Return values of level(), name(), data() and size() are passed to setsockopt(2)
//
//
template<typename ValueType = int>
class generic_option {
public:

    generic_option() = default;
    generic_option(int level, int name, ValueType const &ov) 
        :   level_(level),
            name_(name),
            size_(sizeof ov),
            optval_(ov)
        {}
    generic_option(int level, int name, ValueType const &ov, size_t size) 
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


template<int Level, int Name, typename ValueType = int>
class basic_option {
public:

    basic_option() = default;

    basic_option(ValueType const& value) : optval_(value)
    {
    }

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
    void *data(Protocol const &p)
    {
        // Should return a pointer that is convertible to void*
        return reinterpret_cast<void *>(&optval_);
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
        return sizeof(optval_);
    }

    template <typename Protocol>
    void resize(Protocol const& p, std::size_t s)
    {
        if (s != sizeof(optval_)) {
            std::length_error ex("basic_option resize error");
            boost::throw_exception(ex);
        }
    }

    void set_value(ValueType const& val)
    {
        optval_ = val;
    }

    ValueType const& get_value() const
    {
        return optval_;
    }

private:
    ValueType optval_;
};


//
// Socket option with template parameters of arguments to setsockopt(2)
//
template<int Level, int Name, bool Init = true>
class binary_option {
public:
    binary_option() = default;
    binary_option(bool ov) : optval_(ov ? 1 : 0) {}
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
    void *data(Protocol const &p)
    {
        return reinterpret_cast<void *>(&optval_);
    }
    
    template<typename Protocol>
    void const *data(Protocol const &p) const
    {
        return reinterpret_cast<void const *>(&optval_);
    }

    template<typename Protocol>
    int size(Protocol const &p) const
    {
        return sizeof(optval_);
    }
    
    template <typename Protocol>
    void resize(Protocol const& p, std::size_t s)
    {
        if (s != sizeof(optval_)) {
            std::length_error ex("basic_option resize error");
            boost::throw_exception(ex);
        }
    }

    void set_value(bool val)
    {
        optval_ = val ? 1 : 0;
    }

    bool get_value() const
    {
        return optval_ ? true : false;
    }

private:
    int optval_ = Init;
};


//
// Socket option only for packet socket
//
class packet_socket_option {
public:

    typedef struct packet_mreq option_type;
    typedef void(*mreq_functor_t)(option_type&);
    
    packet_socket_option() : name_(enable)
    {
    }

    packet_socket_option(bool name) 
        : name_(name ? enable : disable)
    {
    }

    // MreqFunctor functor can change the underlying option structure
    template<typename MreqFunctor>
    packet_socket_option(bool name, MreqFunctor functor)
        : name_(name ? enable : disable)
    {
        std::memset(&mreq_, 0, sizeof mreq_);
        // Functor is used for setting packet_mreq data up
        functor(mreq_);
    }

    template<typename Protocol>
    int level(Protocol const& p) const
    {
        return SOL_PACKET;
    }

    template<typename Protocol>
    int name(Protocol const& p) const
    {
        return name_;
    }

    template<typename Protocol>
    void const* data(Protocol const& p) const
    {
        return reinterpret_cast<void const*>(&mreq_);
    }

    template<typename Protocol>
    int size(Protocol const& p) const
    {
        return sizeof mreq_;
    }
        
private:
    enum name_boolean_t 
        { enable = PACKET_ADD_MEMBERSHIP, disable = PACKET_DROP_MEMBERSHIP };
    int name_;
    option_type mreq_;  
};


// Option functor for packet_socket_option
inline namespace packet_option_functor {

// packet_socket_option guarantees that `option_type& mreq`, 
//      the first argument of mreq functor is cleared to zero
void ps_opt_promisc(packet_socket_option::option_type& mreq, int if_index)
{
    mreq.mr_type = PACKET_MR_PROMISC;
    mreq.mr_ifindex = if_index;
}

}   // inline namespace packet_option_functor


// Binary option for IP_HDRINCL
template<bool Init>
using ip_hdrincl = binary_option<SOL_IP, IP_HDRINCL, Init>;
// Above is equal to arex::basic_option<int> iphdrincl(SOL_IP, IP_HDRINCL, 1);


} // namespace arex
} // namespace ip
} // namespace asio
} // namespace boost

#endif  // BOOST_ASIO_EXT_OPTIONS_HPP_
