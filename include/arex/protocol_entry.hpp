#ifndef BOOST_ASIO_AREX_PROTOCOL_ENTRY_HPP
#define BOOST_ASIO_AREX_PROTOCOL_ENTRY_HPP


// Start arex namespace
AREX_BEGIN_NAMESPACE


//
// Protocol entry of OSI layer 4 (Transport layer)
//
// Exception: If a given entry doesn't exist, throw an exception in ctors
//              or
//            operator= throws an exception if a copy of std::string fails
//
class protocol_entry {
public:

    enum { name_buffer_length = 1024 };
    typedef struct protoent entry_type;
    typedef std::vector<std::string> aliases_list_t;

    protocol_entry() : protocol_number_(0)
    {
    }

    protocol_entry(protocol_entry const& other)
        :   protocol_number_(other.protocol_number_),
            official_name_(other.official_name_), 
            aliases_(other.aliases_)
    {
    }

    explicit protocol_entry(int protocol)
    {
        char name_buf_[name_buffer_length];
        entry_type result_buf_, *result_;
    
        int retval = getprotobynumber_r(
            protocol,
            &result_buf_,
            name_buf_,
            sizeof name_buf_,
            &result_
        );
       
        if ( result_ == nullptr || retval != 0 )
            throw_error(retval);

        store_entry_elem(result_buf_);
    }
    
    explicit protocol_entry(std::string const& proto_name)
    {
        char name_buf_[name_buffer_length];
        entry_type result_buf_, *result_;

        int retval = getprotobyname_r(
            proto_name.c_str(),
            &result_buf_,
            name_buf_,
            sizeof name_buf_,
            &result_
        );
       
        if ( result_ == nullptr || retval != 0 )
            throw_error(retval);
        
        store_entry_elem(result_buf_);
    }

    // Protocol number in host byte order
    int number() const
    {
        return protocol_number_;
    }

    // Official name of the protocol
    std::string const& name() const
    {
        return official_name_;
    }

    // A list of alternate names
    aliases_list_t const& aliases() const
    {
        return aliases_;
    }

    protocol_entry& operator=(protocol_entry const& rhs)
    {
        protocol_number_ = rhs.protocol_number_;
        official_name_   = rhs.official_name_;
        aliases_         = rhs.aliases_;
        return *this;
    }

    bool operator==(protocol_entry const& rhs) const
    {
        return (number() == rhs.number());
    }
    
    bool operator!=(protocol_entry const& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator<(protocol_entry const& rhs) const
    {
        return (number() < rhs.number());
    }

    bool operator>(protocol_entry const& rhs) const
    {
        return (number() > rhs.number());
    }

    bool operator<=(protocol_entry const& rhs) const
    {
        return (*this < rhs || *this == rhs);
    }
    
    bool operator>=(protocol_entry const& rhs) const
    {
        return (*this > rhs || *this == rhs);
    }

private:

    static void throw_error(int error_value)
    {
        boost::asio::detail::throw_error(
            boost::system::error_code(error_value, boost::system::system_category())
        );
    }

    void store_entry_elem(entry_type const& ent)
    {
        aliases_list_t tmp;
        for ( char **p = ent.p_aliases; *p != nullptr; ++p )
            tmp.push_back(*p);
        aliases_.swap(tmp);
        protocol_number_ = ent.p_proto;
        official_name_   = ent.p_name;
    }

    int protocol_number_;
    std::string official_name_;
    aliases_list_t aliases_;
};


// End arex namespace
AREX_END_NAMESPACE


#endif  // BOOST_ASIO_AREX_PROTOCOL_ENTRY_HPP
