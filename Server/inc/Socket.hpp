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
#include "../../Conf/incs/parse_config/conf_parser.hpp"


class Socket {
public:
    Socket();
    ~Socket();

    void setupServerSocket(std::vector < Server_storage > &_servers, std::vector <std::pair <Socket , Server_storage > > &serverList);
    int get_fd() const;
    struct sockaddr_in get_address() const;
    bool get_already_bind() const;

private:
    int socket_fd;
    struct sockaddr_in socket_address;
    bool already_bind;
    
};

#endif
