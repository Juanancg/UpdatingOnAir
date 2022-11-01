# Client - Server updater exercise

## Libs used

### Boost

```sh
sudo apt install libboost-dev
sudo apt install libboost-all-dev
```

## Compile

### Client

```sh
# For the client executable
g++ main.cpp TCPClient.cpp -o client -lpthread -lboost_system -ldl

# For the shared library
g++ -fPIC -shared functional_v1_00.cpp -o functional_v1_00.so
```

### Server

```sh
g++ main.cpp TCPServer.cpp TCPConnection.cpp -o server -lpthread -lboost_system
```

## References

* For linux shared libraries management <https://tldp.org/HOWTO/html_single/C++-dlopen/#externC>
* For compile shared libraries in Linux <https://stackoverflow.com/questions/496664/c-dynamic-shared-library-on-linux>
* For boost TCP connections <https://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/examples/cpp11_examples.html>
