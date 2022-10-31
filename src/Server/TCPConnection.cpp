#include "TCPConnection.h"

namespace nexthink {

TCPConnection::TCPConnection(boost::asio::io_service& io_service) : socket_(io_service) {
}

boost::asio::ip::tcp::socket& TCPConnection::getSocket() {
    return socket_;
}

void TCPConnection::start() {
    read();
}

void TCPConnection::read() {
    socket_.async_read_some(
        boost::asio::buffer(data_received_, max_length),
        boost::bind(&TCPConnection::handleRead,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void TCPConnection::write() {
    socket_.async_write_some(
        boost::asio::buffer(msg_to_send_, msg_to_send_.size()),
        boost::bind(&TCPConnection::handleWrite,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void TCPConnection::handleRead(const boost::system::error_code& err_code, const size_t bytes_transferred) {
    if (err_code) {
        std::cerr << "error: " << err_code.message() << std::endl;
        socket_.close();
    } else {
        std::cout << data_received_ << std::endl;
    }
    write();
}

void TCPConnection::handleWrite(const boost::system::error_code& err_code, const size_t bytes_transferred) {
    if (err_code) {
        std::cerr << "error: " << err_code.message() << std::endl;
        socket_.close();
    } else {
        std::cout << "Sent: " << bytes_transferred << " bytes." << std::endl;
    }
}

}  // namespace nexthink