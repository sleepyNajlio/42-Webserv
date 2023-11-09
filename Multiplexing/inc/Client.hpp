#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include "../../response/response.hpp"
#include "../../request/request.hpp"

class Response;
class Request;

class Client
{
private:
    int client_fd;
    struct sockaddr_in client_address;

public:
    Client(const Client &other);
    Client &operator=(const Client &other);
    Client();
    ~Client();
    int get_fd() const;
    struct sockaddr_in get_address() const;
    Request get_request() const;
    void set_fd(int fd);
    std::vector<Location_storage>::const_iterator locIt;
    Response res;
    Request req;
};