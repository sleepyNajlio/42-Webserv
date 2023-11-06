#include "inc/Client.hpp"

Client::Client() : client_fd(-1)  {
        locIt = std::vector<Location_storage>::const_iterator();
        res = Response(); 
        req = Request(); 
    }

Client::Client(const Client& other) {
    *this = other;
    // Perform a deep copy for non-primitive members if necessary
}

Client& Client::operator=(const Client& other) {
    if (this == &other) {
        // Self-assignment, no action needed
        return *this;
    }

    // Perform member-wise assignment
    client_fd = other.client_fd;
    client_address = other.client_address;
    // Perform deep copy or appropriate assignment for non-trivial members if necessary
    res = other.res;
    req = other.req;
    // Return a reference to the modified object
    return *this;
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