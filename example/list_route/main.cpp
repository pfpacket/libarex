
#define AREX_DEFINE_NAMESPACE arex
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <boost/format.hpp>
#include <arex/route.hpp>

void fatal_error(std::string const& msg, int& exit_code)
{
    exit_code = EXIT_FAILURE;
    std::cerr << msg << std::endl;
}

int main(int argc, char **argv)
{
    int exit_code = EXIT_SUCCESS;
    try {
        if (argc < 2)
            throw std::runtime_error(
                "Too few Arguments\n"
                "Usage: ./route IPv4ADDRESS [-l --list]");

        arex::rd_routing_table rt;
        std::cout << "Kernel Routing Table / " << rt.size() << " routes found:" << std::endl;
        std::cout << boost::format("%-15s\t%-15s\t%-15s\t%-15s")
            % "Destination" % "Gateway" % "Netmask" % "Iface" << std::endl;
        for (auto ri : rt)
            std::cout << boost::format("%-15s\t%-15s\t%-15s\t%-15s") % ri.dest.to_string()
                % ri.gateway.to_string() % ri.netmask.to_string() % ri.ifname << std::endl;

        std::string cmdline(argv[1]);
        if (cmdline != "-l" && cmdline != "--list") {
            auto it = rt.find(cmdline);
            std::cout << "[*] Routing target: " << argv[1] << ": ifname="
                << it->ifname << " dest=" << it->dest.to_string() << std::endl;
        }
    } catch (arex::illegal_address& e) {
        fatal_error(std::string("illegal_address: ") + e.what(), exit_code);
    } catch (std::exception& e) {
        fatal_error(std::string("Exception: ") + e.what(), exit_code);
    }
    return exit_code;
}
