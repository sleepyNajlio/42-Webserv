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
        //copy constructor
        Multiplexing(const Multiplexing& other) {
            // Perform a deep copy of the class members
            *this = other;
        }
        // Copy assignment operator to handle assignment correctly
        Multiplexing& operator=(const Multiplexing& other) {
            if (this != &other) {
                // Perform a deep copy of the class members
                io = other.io;
                maxFd = other.maxFd;
                clients = other.clients;
            }
            return *this;
        }

        struct ioSets getIoSets() const;
        void setupServer(std::vector <std::pair <Socket , Server_storage > > &_server);
        void handleNewConnection(Socket& serverSocket, Server_storage server);
        void WriteData(std::pair < Client , Server_storage > &client);
       
};


