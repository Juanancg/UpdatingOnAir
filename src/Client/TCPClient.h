#ifndef NEXTHINK_SERVER_TCPCLIENT_H_
#define NEXTHINK_SERVER_TCPCLIENT_H_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <string>

namespace nexthink {

class TCPClient {
   public:
    TCPClient(const std::string& address, const unsigned int port);

    void run();

   private:
    void asyncRead();
    void handleRead(const boost::system::error_code error_code, const size_t bytes_transferred, std::shared_ptr<boost::asio::streambuf> received_buffer);
    void asyncWrite();
    void handleWrite(const boost::system::error_code error_code, const size_t bytes_transferred);

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::endpoint endpoint_;

    std::string version_request_ = "Version Request\n";

    bool connected_ = false;
};

}  // namespace nexthink

#endif  // NEXTHINK_SERVER_TCPCLIENT_H_