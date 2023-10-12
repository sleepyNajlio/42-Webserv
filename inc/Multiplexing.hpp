#pragma once

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "Socket.hpp"
#include <fcntl.h>
#include <sys/stat.h>
// #include "request.hpp"
#include "Client.hpp"

class request;
class Socket;

typedef struct ioSets{
    fd_set readSockets;
    fd_set writeSockets;
    fd_set tmpReadSockets;
    fd_set tmpWriteSockets;
}   ioSets;


class Multiplexing {
    private:
        ioSets io;
        int maxFd;
        std::vector< Client > clients;
        
    public:
        Multiplexing();
        ~Multiplexing();

        struct ioSets getIoSets() const;
        void setupServer(Socket& serverSocket);
        void handleNewConnection(Socket& serverSocket);
       
};