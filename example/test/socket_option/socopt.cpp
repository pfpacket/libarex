
#define AREX_DEFINE_NAMESPACE arex
#include <iostream>
#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <arex.hpp>

namespace asio = boost::asio;

void print_opt(struct timeval const& tv)
{
    std::cout << "getopt: sec=" << tv.tv_sec
        << " usec=" << tv.tv_usec << std::endl;
}

int main(int argc, char const* argv[])
{
    asio::io_service io;
    asio::ip::tcp::socket socket(io, asio::ip::tcp::v4());
    typedef arex::basic_option<
        SOL_SOCKET,
        SO_RCVTIMEO,
        struct timeval
    > recvtimeo;

    recvtimeo getopt;
    socket.get_option(getopt);
    print_opt(getopt.get_value());

    struct timeval recvtv = {5, 0};
    recvtimeo opt(recvtv);
    socket.set_option(opt);

    recvtimeo afteropt;
    socket.get_option(afteropt);
    print_opt(afteropt.get_value());

    BOOST_ASSERT(opt.get_value().tv_sec == afteropt.get_value().tv_sec);
    BOOST_ASSERT(opt.get_value().tv_usec == afteropt.get_value().tv_usec);

    return 0;
}
