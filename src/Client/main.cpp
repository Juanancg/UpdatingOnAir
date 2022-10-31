#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

#include "TCPClient.h"

int main(int argc, char **argv) {
    std::cout << "Client started" << std::endl;

    std::string address = "127.0.0.1";
    unsigned int port = 1234;

    boost::asio::io_service io_service;

    nexthink::TCPClient client(address, port);
    client.run();

    return 0;
}