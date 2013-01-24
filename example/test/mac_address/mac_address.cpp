
#include <iostream>
#include <stdexcept>
#include <arex/mac_address.hpp>
#include <arex/ethernet_header.hpp>
#include <boost/assert.hpp>

namespace arex = boost::asio::ip::arex;

int main()
{

    arex::mac_address a("aa:bb:cc:dd:ee:ff");
    arex::mac_address b("aa:bb:cc:dd:ee:ff");
    arex::mac_address c("aa:bb:cc:dd:ee:fe");
    BOOST_ASSERT(a == b);
    BOOST_ASSERT(a != c);
    
    arex::mac_address d(c.to_bytes());
    BOOST_ASSERT(c == d);

    arex::mac_address e(d.to_string());
    BOOST_ASSERT(d == e);
    BOOST_ASSERT(c == e);

    try {
        arex::mac_address("aa:bb:cc:dd:ee:gg");
        std::cout << "[-] " << __FILE__ << ':' << __LINE__ << " Test failed" << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception& e) {
        std::cout << "[*] Exception: " << e.what() << std::endl;
        std::cout << "[*] An expected exception occurred: detection of incorrect address works" << std::endl;
    }

    return EXIT_SUCCESS;
}

