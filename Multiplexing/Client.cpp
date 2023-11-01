#include "inc/Client.hpp"

Client::Client()
{
}

Client::~Client()
{
}

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