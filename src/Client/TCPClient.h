#ifndef NEXTHINK_SERVER_TCPCLIENT_H_
#define NEXTHINK_SERVER_TCPCLIENT_H_

#include <dlfcn.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <string>

namespace nexthink {

struct ServerMessage {
    enum E_ELEM_IDX {
        E_VERSION_IDX = 0,
        E_PATH_IDX = 1,
    };

    int version = 0;
    std::string path = "";

    void reset() {
        version = 0;
        path = "";
    }
};
class TCPClient {
   public:
    TCPClient(const std::string& address, const unsigned int port);

    void runUpdaterSystem();

    void run();

   private:
    typedef int (*getVersion_tdef)();

    void asyncRead();
    void handleRead(const boost::system::error_code error_code, const size_t bytes_transferred, std::shared_ptr<boost::asio::streambuf> received_buffer);
    void asyncWrite();
    void handleWrite(const boost::system::error_code error_code, const size_t bytes_transferred);

    bool decodeMessage(const char* message, ServerMessage& message_decoded);
    void readNewVersion(const char* path);

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::endpoint endpoint_;

    std::string version_request_ = "Version Request\n";

    int current_version_;
    bool updated_;
};

}  // namespace nexthink

#endif  // NEXTHINK_SERVER_TCPCLIENT_H_