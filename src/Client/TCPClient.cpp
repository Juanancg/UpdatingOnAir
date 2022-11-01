#include "TCPClient.h"

#include <chrono>
#include <iomanip>
#include <vector>

namespace nexthink {

TCPClient::TCPClient(const std::string& ip, const unsigned int port) : socket_(io_service_) {
    // This could be included in the initializer list but for readable reasons is here
    boost::asio::ip::address boost_address = boost::asio::ip::address::from_string(ip);
    endpoint_ = boost::asio::ip::tcp::endpoint(boost_address, port);

    current_version_ = 0;
    updated_ = false;
    handler_shared_lib_ = nullptr;
}

TCPClient::~TCPClient() {
    std::cout << "Client Closing library...\n";
    dlclose(handler_shared_lib_);
}

void TCPClient::runUpdaterSystem() {
    asyncWrite();

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
        // std::cout << "Client Read: " << bytes_transferred << " bytes. Message: " << received_buffer;  // TODO: Debug print

        ServerMessage new_msg;

        if (decodeMessage(data, new_msg)) {
            if (current_version_ < new_msg.version) {
                readNewVersion(new_msg.path.c_str());
            }
        } else {
            std::cout << "Client Warning decodeMessage failed" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        socket_.close();
        asyncWrite();
    }
}

void TCPClient::asyncWrite() {
    boost::system::error_code error_code;
    socket_.connect(endpoint_, error_code);

    if (!error_code) {
        socket_.async_write_some(
            boost::asio::buffer(version_request_, version_request_.size()),
            boost::bind(&TCPClient::handleWrite,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else {
        std::cout << "Client Warning Connecting to socket: " << error_code.message() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_REQUEST_TIMER));

        asyncWrite();
    }
}

void TCPClient::handleWrite(const boost::system::error_code error_code, const size_t bytes_transferred) {
    if (error_code) {
        std::cerr << "Client Write Error: " << error_code.message() << " Closing socket" << std::endl;
        socket_.close();
    } else {
        std::cout << "Client Sent "
                  << " (" << bytes_transferred << " bytes): " << version_request_ << std::endl;
        asyncRead();
    }
}

void TCPClient::run() {
    if (updated_) {
        // Call here the function from the shared lib to check if it can run the functional updated code here
        int new_version = getVersion_();

        current_version_ = new_version;
        float version = current_version_ / 100.0;

        std::cout << "Client New Version: " << std::setprecision(2) << std::fixed << version << std::endl;
        updated_ = false;
    }
}

bool TCPClient::decodeMessage(const char* message, ServerMessage& message_decoded) {
    bool successful = false;

    message_decoded.reset();

    std::vector<std::string> splitted_string;
    std::stringstream ss(message);
    std::string item;

    // The message format is VERSION;PATH_TO_SHARED_LIB
    // The code should have to split the message for get each element
    while (std::getline(ss, item, ';')) {
        splitted_string.push_back(item);
    }

    if (splitted_string.size() == ServerMessage::E_TOTAL_ELEMENTS) {
        successful = true;
        message_decoded.version = stoi(splitted_string[ServerMessage::E_VERSION_IDX]);
        message_decoded.path = splitted_string[ServerMessage::E_PATH_IDX];
        message_decoded.path.pop_back();  // For removing \n char

    } else {
        successful = false;
        std::cout << "Client: Couldn't decode Server Message because it has unexpected num of elements (2 expected): " << message << std::endl;
    }

    return successful;
}

void TCPClient::readNewVersion(const char* path) {
    // Open the library
    std::cout << "Client Opening " << path << std::endl;

    handler_shared_lib_ = dlopen(path, RTLD_LAZY);

    if (!handler_shared_lib_) {
        std::cerr << "Client Cannot open library: " << dlerror() << std::endl;
        return;
    }

    // Reset errors
    dlerror();

    // Load the symbol
    getVersion_ = (getVersion_typedef)dlsym(handler_shared_lib_, "getVersion");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Client Cannot load symbol 'getVersion': " << dlsym_error << std::endl;
        dlclose(handler_shared_lib_);
        return;
    }

    updated_ = true;
}

}  // namespace nexthink
