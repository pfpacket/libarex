
#include <iostream>
#include <cstdint>
#include <arex/arex_utils.hpp>
#include <arpa/inet.h>
#include <boost/assert.hpp>

namespace arex = boost::asio::ip::arex;

int main()
{
    std::cout << std::hex;

    std::uint16_t data1 = 0xFFAA;
    std::cout << std::endl << "htons test" << std::endl;
    std::cout << "Data: " << data1 << std::endl;
    std::cout << "arex: " << arex::htons(data1) << std::endl;
    std::cout << "Correct: " << ::htons(data1) << std::endl;
    BOOST_ASSERT(arex::htons(data1) == ::htons(data1));

    std::uint16_t data2 = 0xFFAA;
    std::cout << std::endl << "ntohs test" << std::endl;
    std::cout << "Data: " << data2 << std::endl;
    std::cout << "arex: " << arex::ntohs(data2) << std::endl;
    std::cout << "Correct: " << ::ntohs(data2) << std::endl;
    BOOST_ASSERT(arex::ntohs(data2) == ::ntohs(data2));
    
    std::uint32_t data3 = 0xFAFBFCFD;
    std::cout << std::endl << "htonl test" << std::endl;
    std::cout << "Data: " << data3 << std::endl;
    std::cout << "arex: " << arex::htonl(data3) << std::endl;
    std::cout << "Correct: " << ::htonl(data3) << std::endl;
    BOOST_ASSERT(arex::htonl(data3) == ::htonl(data3));

    std::uint32_t data4 = 0xFAFBFCFD;
    std::cout << std::endl << "ntohl test" << std::endl;
    std::cout << "Data: " << data4 << std::endl;
    std::cout << "arex: " << arex::ntohl(data4) << std::endl;
    std::cout << "Correct: " << ::ntohl(data4) << std::endl;
    BOOST_ASSERT(arex::ntohl(data4) == ::ntohl(data4));

    return 0;
}

