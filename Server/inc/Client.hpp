#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include "../../Request/request.hpp"

class request;

class Client {
    private:
        int client_fd;
        struct sockaddr_in client_address;
    public:
        Client();
        ~Client();
        int get_fd() const;
        struct sockaddr_in get_address() const;
        request get_request() const;
        void set_fd(int fd);
        // request req;

};