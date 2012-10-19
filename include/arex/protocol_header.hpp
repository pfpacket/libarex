#ifndef PROTOCOL_HEADER_HPP
#define PROTOCOL_HEADER_HPP

#include <iostream>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/streambuf.hpp>


namespace boost {
namespace asio {
namespace ip {
namespace arex {


//
// class protocol_header
// An abstrace base class of each protocol headers
//
class protocol_header {
public:
    protocol_header() {}
    virtual ~protocol_header() {}
    
    virtual std::size_t length() const = 0;
    virtual char* get_header() = 0;
    virtual char const* get_header() const = 0;
    
    static u_int32_t address_to_binary(std::string const& straddr)
    {
        return boost::asio::ip::address_v4::from_string(straddr).to_ulong();
    } 
     
    static std::string address_to_string(u_int32_t binaddr)
    {
        return boost::asio::ip::address_v4(binaddr).to_string();
    } 

    template<typename Elem, typename Traits>
    friend std::basic_istream<Elem, Traits>& operator>>(
        std::basic_istream<Elem, Traits> &is,
        protocol_header& header
    );

    template<typename Elem, typename Traits>
    friend std::basic_ostream<Elem, Traits>& operator<<(
        std::basic_ostream<Elem, Traits> &os,
        protocol_header& header
    );

    friend void copy_buffer_to_header(
        protocol_header& header,
        char const* buf
    );
    
    friend bool streambuf_to_header(
        protocol_header& header,
        boost::asio::streambuf const& buf,
        std::size_t offset = 0
    );

//    friend bool overwrite_streambuf(
//        protocol_header const& header,
//        boost::asio::streambuf const& buf,
//        std::size_t offset = 0);

protected:

    static unsigned short checksum(unsigned short *buf, int bufsz)
    {
      unsigned long sum = 0;
        while( bufsz > 1 ) {
            sum += *buf++;
            bufsz -= 2;
        }
        if( bufsz == 1 )
            sum += *(unsigned char *)buf;
        sum = (sum & 0xffff) + (sum >> 16);
        sum = (sum & 0xffff) + (sum >> 16);
        return ~sum;
    }
     
    virtual void prepare_to_read(std::istream&) {}
    virtual void prepare_to_write(std::ostream&) {}
};


template<typename Elem, typename Traits>
std::basic_istream<Elem, Traits>& operator>>(std::basic_istream<Elem, Traits> &is, protocol_header& header)
{
    header.prepare_to_read(is);
    return is.read(header.get_header(), header.length());
}


template<typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits> &os, protocol_header& header)
{
    header.prepare_to_write(os);
    return os.write(header.get_header(), header.length());
}


void copy_buffer_to_header(protocol_header& header, char const* buf)
{
    std::copy(buf, buf + header.length(), header.get_header());
}


// Copy buffer contents to protocol_header
// Return false if buffer length is not enough to read
// Otherwise return true
bool streambuf_to_header(
    protocol_header& header,
    boost::asio::streambuf const& buf, std::size_t offset)
{
    // Length check
    if ( buf.size() < header.length() + offset )
        return false;
    char const* head = 
        boost::asio::buffer_cast<char const*>(buf.data()) + offset;
    std::copy(
        head,
        head + header.length(),
        header.get_header()
    );
    return true;
}


}   // namespace arex
}   // namespace ip
}   // namespace asio
}   // namespace boost


#endif // PROTOCOL_HEADER_HPP

