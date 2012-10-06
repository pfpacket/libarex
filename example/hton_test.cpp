
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <arex/arex_utils.hpp>
#include <arpa/inet.h>
#include <boost/assert.hpp>

using namespace std;
namespace arex = boost::asio::ip::arex;

int main()
{
    cout << hex;

    uint16_t data1 = 0xFFAA;
    cout << endl << "htons test" << endl;
    cout << "Data: " << data1 << endl;
    cout << "arex: " << arex::htons(data1) << endl;
    cout << "Correct: " << ::htons(data1) << endl;
    BOOST_ASSERT(arex::htons(data1) == ::htons(data1));

    uint16_t data2 = 0xFFAA;
    cout << endl << "ntohs test" << endl;
    cout << "Data: " << data2 << endl;
    cout << "arex: " << arex::ntohs(data2) << endl;
    cout << "Correct: " << ::ntohs(data2) << endl;
    BOOST_ASSERT(arex::ntohs(data2) == ::ntohs(data2));
    
    uint32_t data3 = 0xFAFBFCFD;
    cout << endl << "htonl test" << endl;
    cout << "Data: " << data3 << endl;
    cout << "arex: " << arex::htonl(data3) << endl;
    cout << "Correct: " << ::htonl(data3) << endl;
    BOOST_ASSERT(arex::htonl(data3) == ::htonl(data3));

    uint32_t data4 = 0xFAFBFCFD;
    cout << endl << "ntohl test" << endl;
    cout << "Data: " << data4 << endl;
    cout << "arex: " << arex::ntohl(data4) << endl;
    cout << "Correct: " << ::ntohl(data4) << endl;
    BOOST_ASSERT(arex::ntohl(data4) == ::ntohl(data4));

    return 0;
}

