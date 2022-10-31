#include "TCPClient.h"

#include <chrono>
#include <iomanip>
#include <vector>

namespace nexthink {

TCPClient::TCPClient(const std::string& ip, const unsigned int port) : socket_(io_service_) {
    // This could be included in the initializer list but for readable reasons is here
    boost::asio::ip::address boost_address = boost::asio::ip::address::from_string(ip);
    endpoint_ = boost::asio::ip::tcp::endpoint(boost_address, port);
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
        std::cout << "Client Read: " << bytes_transferred << " bytes. Message: " << received_buffer;  // TODO: Debug print
        ServerMessage new_msg;
        if (decodeMessage(data, new_msg)) {
            if (current_version_ < new_msg.version) {
                readNewVersion(new_msg.path.c_str());
            }
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        asyncWrite();
    }
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

void TCPClient::run() {
    if (updated_) {
        float version = current_version_ / 100.0;
        std::cout << "Client New Version: " << std::setprecision(2) << std::fixed << version << std::endl;
        updated_ = false;
        // TODO: Lets see if you can call here the function from the shared lib
    }
}

bool TCPClient::decodeMessage(const char* message, ServerMessage& message_decoded) {
    bool successful = false;

    message_decoded.reset();

    std::vector<std::string> splitted_string;
    std::stringstream ss(message);
    std::string item;

    while (std::getline(ss, item, ';')) {
        splitted_string.push_back(item);
    }

    if (splitted_string.size() == 2) {  // TODO: This to a constant inside ServerMessage
        successful = true;
        message_decoded.version = stoi(splitted_string[ServerMessage::E_VERSION_IDX]);
        message_decoded.path = splitted_string[ServerMessage::E_PATH_IDX];
        message_decoded.path.pop_back();  // For removing \n char

    } else {
        successful = false;
        std::cout << "Client: Couldn't decode Server Message: " << message << std::endl;
    }

    return successful;
}

void TCPClient::readNewVersion(const char* path) {
    // Open the library
    std::cout << "Client Opening " << path << std::endl;

    void* handle = dlopen(path, RTLD_LAZY);

    if (!handle) {
        std::cerr << "Client Cannot open library: " << dlerror() << std::endl;
        return;
    }

    // Reset errors
    dlerror();

    // Load the symbol
    getVersion_tdef getVersion = (getVersion_tdef)dlsym(handle, "getVersion");  // TODO: Here test if we can save it to a private attribute for using it in run()

    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Client Cannot load symbol 'getVersion': " << dlsym_error << std::endl;
        dlclose(handle);
        return;
    }

    int new_version = getVersion();
    current_version_ = new_version;
    updated_ = true;

    // TODO: Manage the closing in the stop or in destructor
    // close the library
    std::cout << "Client Closing library...\n";
    dlclose(handle);
}

}  // namespace nexthink
