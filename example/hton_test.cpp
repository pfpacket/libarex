
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <arex/arex_utils.hpp>
#include <arpa/inet.h>

using namespace std;
namespace arex = boost::asio::ip::arex;

int main()
{
    uint32_t data = 0xFAFBFCFD;
    uint32_t bin  = arex::htonl(data);
    cout << hex << "Before: " << data << endl;
    cout << (bin & 0xFF000000);
    cout << (bin & 0x00FF0000);
    cout << (bin & 0x0000FF00);
    cout << (bin & 0x000000FF);
    cout << endl;
    cout << bin << endl;
    cout << "Correct: " << ::htonl(data) << endl;

    cout << " ----- htons -------" << endl;
    uint16_t data2 = 0xFFAA;
    cout << "arex: " << arex::htons(data2) << endl;
    cout << "Correct: " << ::htons(data2) << endl;
    return 0;
}

