#include "TCPServer.h"

namespace nexthink {

TCPServer::TCPServer(const std::string address, const unsigned int port, boost::asio::io_service& io_service) : acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(address), port)),
                                                                                                                io_service_(io_service) {
    startAccept();
}

void TCPServer::startAccept() {
    // Create a new TCPConnection
    std::shared_ptr<TCPConnection> connection = std::make_shared<TCPConnection>(io_service_);

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

}  // namespace nexthink