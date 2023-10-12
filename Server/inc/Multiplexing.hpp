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
#include "Client.hpp"
#include "../../Conf/incs/parse_config/conf_parser.hpp"

class request;
class Socket;

typedef struct ioSets{
    fd_set readSockets;
    // int readSocketsSize;
    fd_set writeSockets;
    // int writeSocketsSize;
    fd_set tmpReadSockets;
    fd_set tmpWriteSockets;
}   ioSets;

class Response {
    public:
        bool headerSent;
        // int videoFile;
        std::string responseHeader;
        int resBodyFd;
        int resBodySize;
        Response();
        ~Response();
        

};

class Multiplexing {
    private:
        int reqBodyFd;
        ioSets io;
        int maxFd;
        std::vector<std::pair < Client, Server_storage > > clients;
        // std::vector <std::pair <int , Server_storage > > _servers;
    public:
        Multiplexing();
        ~Multiplexing();
        std::vector<std::pair < Client, Server_storage > > getClients() const;
        struct ioSets getIoSets() const;
        int getBodyFd() const;

        void reader(unsigned char *buffer, Client &client, Server_storage server, ssize_t bytesRead);
        void setupServer(std::vector <std::pair <Socket , Server_storage > > _server);
        void handleNewConnection(Socket& serverSocket, Server_storage server);
       
};