#include "TCPClient.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <vector>

namespace nexthink {

TCPClient::TCPClient(const std::string& ip, const unsigned int port) : socket_(io_service_) {
    // This could be included in the initializer list but for readable reasons is here
    boost::asio::ip::address boost_address = boost::asio::ip::address::from_string(ip);
    endpoint_ = boost::asio::ip::tcp::endpoint(boost_address, port);

    handler_shared_lib_ = nullptr;
    getInfoFromMetadata();

    // This is updated due to the requirement:
    // Prints its version to the console whenever the application is launched or after an update
    updated_ = true;
}

TCPClient::~TCPClient() {
    std::cout << "Client Closing library..." << std::endl;
    dlclose(handler_shared_lib_);
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
    if (!error_code) {
        const char* data = boost::asio::buffer_cast<const char*>(received_buffer->data());

        ServerMessage new_msg;

        if (decodeMessage(data, new_msg)) {
            if (current_version_ < new_msg.version) {
                readNewVersion(new_msg);
            }
        } else {
            std::cout << "Client Warning decodeMessage failed" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_REQUEST_TIMER));
        socket_.close();
        asyncWrite();

    } else {
        std::cerr << "Client Read Error: " << error_code.message() << " Closing socket" << std::endl;
        socket_.close();
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

        // Wait before another try to connect to the socket
        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_REQUEST_TIMER));
        asyncWrite();
    }
}

void TCPClient::handleWrite(const boost::system::error_code error_code, const size_t bytes_transferred) {
    if (!error_code) {
        std::cout << "Client Sent "
                  << "(" << bytes_transferred << " bytes): " << version_request_ << std::endl;
        asyncRead();

    } else {
        std::cerr << "Client Write Error: " << error_code.message() << " Closing socket" << std::endl;
        socket_.close();
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

void TCPClient::readNewVersion(const ServerMessage new_msg) {
    // Get the info from the shared lib and save it to the private pointer to the function
    readSharedLib(new_msg.path.c_str());
    updated_ = true;

    // Save the new info to the metadata file for future executions
    saveDataToMetadata(new_msg);
}

void TCPClient::readSharedLib(const char* path) {
    // Open the library
    // std::cout << "Client Opening " << path << std::endl; // NOTE: DEBUG PRINTS
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
}

void TCPClient::getInfoFromMetadata() {
    std::string line;
    std::ifstream metadata_file(metadata_file_);
    if (metadata_file.is_open()) {
        // First line: current version
        getline(metadata_file, line);
        current_version_ = stoi(line);

        // Second line: Path to the shared lib
        getline(metadata_file, line);
        std::string path_to_shared_lib = line;

        // Get the info from the shared lib and save it to the private pointer to the function
        readSharedLib(path_to_shared_lib.c_str());
        metadata_file.close();
    }
}

void TCPClient::saveDataToMetadata(const ServerMessage new_msg) {
    std::ofstream client_version_file(metadata_file_);
    if (client_version_file.is_open()) {
        client_version_file << new_msg.version;
        client_version_file << "\n";
        client_version_file << new_msg.path;
        client_version_file.close();
    }
}

}  // namespace nexthink
