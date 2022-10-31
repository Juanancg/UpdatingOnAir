#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

#include "TCPServer.h"

int main(int argc, char **argv) {
    std::cout << "Asynch TCP Server updater" << std::endl;

    // if (argc != 3) {
    //     std::cout << "2 parameters are mandatory: ./server ip port" << std::endl;
    //     return -1;
    // }
    // std::string address = argv[1];
    // unsigned int port = std::atoi(argv[2]);

    std::string address = "127.0.0.1";
    unsigned int port = 1234;

    boost::asio::io_service io_service;

    nexthink::TCPServer server(address, port, io_service);
    io_service.run();

    return 0;
}