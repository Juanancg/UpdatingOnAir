#include "TCPClient.h"

namespace nexthink {

TCPClient::TCPClient(const std::string& ip, const unsigned int port) : socket_(io_service_) {
    // This could be included in the initializer list but for readable reasons is here
    boost::asio::ip::address boost_address = boost::asio::ip::address::from_string(ip);
    endpoint_ = boost::asio::ip::tcp::endpoint(boost_address, port);
}

void TCPClient::run() {
    boost::system::error_code error_code;
    socket_.connect(endpoint_, error_code);

    if (!error_code) {
        asyncWrite();
    }

    io_service_.run();
}

void TCPClient::asyncRead() {
    std::shared_ptr<boost::asio::streambuf> streambuf = std::make_shared<boost::asio::streambuf>();

    std::string buffer_str;

    boost::asio::async_read_until(
        socket_, *streambuf, '\n',
        boost::bind(
            &TCPClient::handleRead,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred,
            streambuf));
}

void TCPClient::handleRead(const boost::system::error_code error_code, const size_t bytes_transferred, std::shared_ptr<boost::asio::streambuf> received_buffer) {
    if (error_code) {
        std::cerr << "Client Read Error: " << error_code.message() << " Closing socket" << std::endl;
        socket_.close();
    } else {
        const char* data = boost::asio::buffer_cast<const char*>(received_buffer->data());
        std::cout << "Client Read: " << bytes_transferred << " bytes. -> " << received_buffer << std::endl;
        // TODO sleep and new socket creation
        // asyncWrite();
    }
}

void TCPClient::asyncWrite() {
    socket_.async_write_some(
        boost::asio::buffer(version_request_, version_request_.size()),
        boost::bind(&TCPClient::handleWrite,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void TCPClient::handleWrite(const boost::system::error_code error_code, const size_t bytes_transferred) {
    if (error_code) {
        std::cerr << "Client Write Error: " << error_code.message() << " Closing socket" << std::endl;
        socket_.close();
    } else {
        std::cout << "Client Sent: " << bytes_transferred << " bytes." << std::endl;
        asyncRead();
    }
}

// void TCPClient::functional() {
//     std::cout << "Current version: " << std::endl;
//     while (true) {
//
//         if (new_version) {
//             std::cout << "New version: " << std::endl;
//         }
//
//     }
// }

}  // namespace nexthink
