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

### Future Improvements

* Instead of having the shared lib .so file in the machine, the Server could send a url where the .so file is uploaded, so that the client downloads it. In my opinion, the current implementation is enought for resolving the exercise, but the proposed approach could be similar to an operational service, where the final clients doesn't have the new updated .so files.
* Client:
  * Catch the signal of SIGINT in order to call TCPClient destructor so that the dclose is called to avoid memory leaks
  * Save in a .txt the current version and path of the dll in order to has a history

* Server:
  * Following something similar to the TCPClient, manage to get from console or file the new version, in order to not having to shutdown the server to get an updated version

## References

* For linux shared libraries management <https://tldp.org/HOWTO/html_single/C++-dlopen/#externC>
* For compile shared libraries in Linux <https://stackoverflow.com/questions/496664/c-dynamic-shared-library-on-linux>
* For boost TCP connections <https://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/examples/cpp11_examples.html>
