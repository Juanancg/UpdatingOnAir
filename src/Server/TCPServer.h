#ifndef NEXTHINK_SERVER_TCPSERVER_H_
#define NEXTHINK_SERVER_TCPSERVER_H_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <string>

#include "TCPConnection.h"

namespace nexthink {

class TCPServer {
   public:
    TCPServer(const std::string address,
              const unsigned int port,
              const std::string message_to_send,
              boost::asio::io_service& io_service);

   private:
    void startAccept();

    void handleAccept(const boost::system::error_code& err_code, std::shared_ptr<TCPConnection> connection);

    bool messageSanityCheck(const std::string message_to_send);

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_service& io_service_;

    std::string message_version_ = "100;./SharedLibs/functional_v1_00.so\n";
};

}  // namespace nexthink

#endif  // NEXTHINK_SERVER_TCPSERVER_H_