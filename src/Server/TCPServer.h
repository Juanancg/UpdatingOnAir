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
    TCPServer(const std::string address, const unsigned int port, boost::asio::io_service& io_service);

   private:
    void startAccept();

    void handleAccept(const boost::system::error_code& err_code, std::shared_ptr<TCPConnection> connection);

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_service& io_service_;
};

}  // namespace nexthink

#endif // NEXTHINK_SERVER_TCPSERVER_H_