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

    std::thread client_thread{[&client]() { client.runUpdaterSystem(); }};

    client.run();

    while (true) {
        std::cout << "Client Working... " << std::endl;
        client.run();
        sleep(1);
    }

    // client.close();
    client_thread.join();

    return 0;
}