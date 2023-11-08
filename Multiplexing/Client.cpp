#include "inc/Client.hpp"

Client::Client() : client_fd(-1)  {
        locIt = std::vector<Location_storage>::const_iterator();
        res = Response(); 
        req = Request(); 
    }

Client::Client(const Client& other) {
    *this = other;
}

Client& Client::operator=(const Client& other) 
{
    if (this == &other)
        return *this;

    client_fd = other.client_fd;
    client_address = other.client_address;
    res = other.res;
    req = other.req;
    return *this;
}

Client::~Client(){}

Request Client::get_request() const
{
    return this->req;
}

int Client::get_fd() const
{
    return this->client_fd;
}

struct sockaddr_in Client::get_address() const
{
    return this->client_address;
}

void Client::set_fd(int fd)
{
    this->client_fd = fd;
}