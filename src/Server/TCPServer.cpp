#include "TCPServer.h"

namespace nexthink {

TCPServer::TCPServer(const std::string address,
                     const unsigned int port,
                     const std::string message_to_send,
                     boost::asio::io_service& io_service) : acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(address), port)),
                                                            io_service_(io_service) {
    if (messageSanityCheck(message_to_send)) {
        message_version_ = message_to_send;
    }

    startAccept();
}

void TCPServer::startAccept() {
    // Create a new TCPConnection
    std::shared_ptr<TCPConnection> connection = std::make_shared<TCPConnection>(io_service_);

    connection->setMessage(message_version_);

    // Accept connection asynch
    acceptor_.async_accept(connection->getSocket(),
                           boost::bind(&TCPServer::handleAccept, this,
                                       boost::asio::placeholders::error, connection));
}

void TCPServer::handleAccept(const boost::system::error_code& err_code, std::shared_ptr<TCPConnection> connection) {
    if (!err_code) {
        connection->start();
    } else {
        std::cerr << err_code.message() << std::endl;
    }

    startAccept();
}

bool TCPServer::messageSanityCheck(const std::string message_to_send) {
    bool goodFormat = false;
    std::vector<std::string> splitted_string;
    std::stringstream ss(message_to_send);
    std::string item;

    while (std::getline(ss, item, ';')) {
        splitted_string.push_back(item);
    }

    if (splitted_string.size() == 2) {
        goodFormat = true;
    } else {
        std::cout << "Server Wrong message format: " << message_to_send << ". Sending the message by default" << std::endl;
    }

    return goodFormat;
}

}  // namespace nexthink