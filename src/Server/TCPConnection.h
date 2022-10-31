#ifndef NEXTHINK_SERVER_TCPCONNECTION_H_
#define NEXTHINK_SERVER_TCPCONNECTION_H_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <string>

namespace nexthink {

class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
   public:
    TCPConnection(boost::asio::io_service& io_service);

    boost::asio::ip::tcp::socket& getSocket();

    void start();

   private:
    void read();

    void write();

    void handleRead(const boost::system::error_code& err_code, const size_t bytes_transferred);

    void handleWrite(const boost::system::error_code& err_code, const size_t bytes_transferred);

    boost::asio::ip::tcp::socket socket_;
    std::string msg_to_send_ = "100;./SharedLibs/functional_v1_00.so\n";

    enum { max_length = 128 };
    char data_received_[max_length];
};

}  // namespace nexthink

#endif  // NEXTHINK_SERVER_TCPCONNECTION_H_