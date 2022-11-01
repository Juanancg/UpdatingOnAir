#!/bin/bash

function buildClient() {
    cp src/Client/metadata.txt bin/metadata.txt
    g++ src/Client/main.cpp src/Client/TCPClient.cpp -o bin/client -lpthread -lboost_system -ldl
}

function buildServer() {
    g++ src/Server/main.cpp src/Server/TCPServer.cpp src/Server/TCPConnection.cpp -o bin/server -lpthread -lboost_system -ldl
}

function buildSharedLibs() {
    g++ -fPIC -shared src/Client/SharedLibs/functional_v1_00.cpp -o bin/SharedLibs/functional_v1_00.so
    g++ -fPIC -shared src/Client/SharedLibs/functional_v1_01.cpp -o bin/SharedLibs/functional_v1_01.so
    g++ -fPIC -shared src/Client/SharedLibs/functional_v1_02.cpp -o bin/SharedLibs/functional_v1_02.so
    g++ -fPIC -shared src/Client/SharedLibs/functional_v2_00.cpp -o bin/SharedLibs/functional_v2_00.so
}
