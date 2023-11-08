#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "../../Conf/incs/server_storage.hpp"

class Socket {
    public:
        Socket();
        ~Socket();

        void setupServerSocket(std::vector < Server_storage > &_servers, std::vector <std::pair <Socket , Server_storage > > &serverList);
        int get_fd() const;
        struct sockaddr_in get_address() const;
        bool get_already_bind() const;
        std::vector <int> get_sockets();

private:
        int socket_fd;
        struct sockaddr_in socket_address;
        bool already_bind;
    
};

#endif
