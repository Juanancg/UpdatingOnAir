# Client - Server updater exercise

## Libs used

### Boost

```sh
sudo apt install libboost-dev
sudo apt install libboost-all-dev
```

## Compile

In order to compile it succesfully, you will need to have a "bin" directory and inside it a "SharedLibs" directory. If you clone this repo, you wouldn't need to create this directories.

```sh
source utils.sh
buildClient
buildSharedLibs
buildServer
```

## Execution

Open two shells, one for the server and other for the client.

### Client

```sh
# Go to your bin directory
cd bin
./client
```

### Server

```sh
# Go to your bin directory
cd bin

# Execute server with the version and the path to the shared lib
./server 100 ./SharedLibs/functional_v1_00.so

# You can ctrl+C and send another different version
./server 101 ./SharedLibs/functional_v1_01.so
```

## Future Improvements

* Instead of having the shared lib .so file in the machine, the Server could send a url where the .so file is uploaded, so that the client downloads it. In my opinion, the current implementation is enought for resolving the exercise, but the proposed approach could be similar to an operational service, where the final clients doesn't have the new updated .so files.
* Client:
  * Catch the signal of SIGINT in order to call TCPClient destructor so that the dclose is called to avoid memory leaks

* Server:
  * Following a similar approach as how the TCPClient works with threads, manage to get the new version in real time from console or file, without the need of shuting down the server and then executing with different parameters to send an updated version

* Use Makefile, CMake or meson to manage better the compilation and generation of the bin files
* Use dockers for avoiding to install the external dependencies in the developer machine.
* The code of TCPConnection and TCPClient could be refactor using OOP and Inheritance.

## References

* For linux shared libraries management <https://tldp.org/HOWTO/html_single/C++-dlopen/#externC>
* For compile shared libraries in Linux <https://stackoverflow.com/questions/496664/c-dynamic-shared-library-on-linux>
* For boost TCP connections <https://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/examples/cpp11_examples.html>
