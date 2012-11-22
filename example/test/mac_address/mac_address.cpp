
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <arex/mac_address.hpp>
#include <arex/ethernet_header.hpp>

using namespace std;
namespace arex = boost::asio::ip::arex;

int main(int argc, char **argv)
{
    if( argc < 2 ) {
        cout << "usage: ./a.out MAC_ADDRSS" << endl;
        return 1;
    }
    arex::mac_address addr(argv[1]);
    arex::mac_address addr2("aa:bb:cc:dd:ee:ff");
    cout << "MAC address in bynary: ";
    for( auto i : addr.to_bytes() )
        printf("%0x", i);
    cout << endl;
    arex::mac_address addr3(addr.to_bytes());
    cout << "MAC address you typed is / " << addr3 << endl;
    cout << "addr == addr2 : " << boolalpha << (addr == addr2) << endl;
    cout << "addr == addr3 : " << boolalpha << (addr == addr3) << endl;

    arex::ethernet_header ethh;
    ethh.source(addr);
    ethh.dest(addr2);
    ethh.type(0);
    cout << "Header source: " << ethh.source() << endl;
    cout << "Header dest: " << ethh.dest() << endl;
    cout << "Header type: " << ethh.type() << endl;
    return 0;
}

