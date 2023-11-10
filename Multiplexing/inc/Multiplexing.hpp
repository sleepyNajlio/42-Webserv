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
#include "../../response/response.hpp"
#include "Client.hpp"
#include "../../Conf/incs/server_storage.hpp"
#include "../../Conf/incs/location_storage.hpp"

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
        std::vector<std::pair < Client, Server_storage > > clients;

    public:
        Multiplexing();
        ~Multiplexing();
        Multiplexing(const Multiplexing& other) {
            *this = other;
        }
        Multiplexing& operator=(const Multiplexing& other) {
            if (this != &other) {
                io = other.io;
                maxFd = other.maxFd;
                clients = other.clients;
            }
            return *this;
        }
        void deleteTempFile(Client &client);
        struct ioSets getIoSets() const;
        void setupServer(std::vector <std::pair <Socket , Server_storage > > _server);
        void handleNewConnection(Socket& serverSocket, Server_storage server);
       
};


