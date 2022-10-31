#include "TCPClient.h"

#include <iomanip>
#include <vector>

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
        ServerMessage new_msg;
        if (decodeMessage(data, new_msg)) {
            if (new_msg.version != current_version_) {
                readNewVersion(new_msg.path.c_str());
            }
        }
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
//         if (new_version) {
//             std::cout << "New version: " << std::endl;
//         }
//     }
// }

bool TCPClient::decodeMessage(const char* message, ServerMessage& message_decoded) {
    bool successful = false;

    message_decoded.reset();

    std::vector<std::string> splitted_string;
    std::stringstream ss(message);
    std::string item;

    while (std::getline(ss, item, ';')) {
        splitted_string.push_back(item);
    }

    if (splitted_string.size() == 2) {
        successful = true;
        message_decoded.version = stoi(splitted_string[ServerMessage::E_VERSION_IDX]);
        message_decoded.path = splitted_string[ServerMessage::E_PATH_IDX];
        message_decoded.path.pop_back();  // To remove \n char

    } else {
        successful = false;
        std::cout << "Client: Couldn't decode Server Message: " << message << std::endl;
    }

    return successful;
}

void TCPClient::readNewVersion(const char* path) {
    // open the library
    std::cout << "Client Opening " << path << std::endl;

    void* handle = dlopen(path, RTLD_LAZY);

    if (!handle) {
        std::cerr << "Client Cannot open library: " << dlerror() << std::endl;
        return;
    }

    // load the symbol
    std::cout << "Client Loading symbol getVersion..." << std::endl;

    // reset errors
    dlerror();
    getVersion_tdef getVersion = (getVersion_tdef)dlsym(handle, "getVersion");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Client Cannot load symbol 'getVersion': " << dlsym_error << std::endl;
        dlclose(handle);
        return;
    }

    // use it to do the calculation
    std::cout << "Client Calling getVersion...\n";
    int new_version = getVersion();
    current_version_ = new_version;
    float version = new_version / 100;
    std::cout << "Client New Version: " << std::setprecision(2) << std::fixed << version << std::endl;

    // close the library
    std::cout << "Client Closing library...\n";
    dlclose(handle);
}

}  // namespace nexthink
