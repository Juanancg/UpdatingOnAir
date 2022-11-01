#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

#include "TCPServer.h"

int main(int argc, char** argv) {
    std::cout << "Asynch TCP Server updater" << std::endl;

    // For accepting new versions without having to touch the code
    std::string message = "";
    if (argc == 3) {
        std::string version = argv[1];
        std::string path = argv[2];
        message = version + ";" + path + "\n";
    }

    std::string address = "127.0.0.1";
    unsigned int port = 1234;

    boost::asio::io_service io_service;

    nexthink::TCPServer server(address, port, message, io_service);
    io_service.run();

    return 0;
}