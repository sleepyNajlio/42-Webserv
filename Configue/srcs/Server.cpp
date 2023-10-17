#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"

Server::Server() {}

Server::Server(Server const & src) {
    *this = src;
}

void Server::set_locations(Location value)
{
    locations.push_back(value);
}
Server & Server::operator=(Server const & obj) {
    if (this != &obj) {
        this->server_name = obj.server_name;
        this->root = obj.root;
        this->locations = obj.locations;
        this->port = obj.port; 
    }
    return *this;
}

Server::~Server() {}



std::string Server::get_root(){
    return this->root;
}

std::string Server::get_server_name(){
    return this->server_name;
}

int    Server::get_port(){
    return this->port;
}

std::string Server::get_path(int i){
    return this->locations[i].path;
}

std::string Server::get_fastcgi_pass(int i){
    return this->locations[i].fastcgi_pass;
}

std::string Server::get_location_root(int i){
    return this->locations[i].root;
}

std::string Server::get_expires(int i){
    return this->locations[i].expires;
}

void        Server::set_root(std::string value){
    this->root = value;
}

void        Server::set_server_name(std::string value){
    this->server_name = value;
}

void        Server::set_port(int value){
        this->port = value;
}

std::vector <Location> Server::get_locations()
{
    return this->locations;
}

std::ostream&   operator<<( std::ostream& out,  Server& obj ) {
    std::vector <Location> temp = obj.get_locations();
    
    out << "- port        : " << obj.get_port() << std::endl
        << "- server_name : " << obj.get_server_name() << std::endl
        << "- root        : " << obj.get_root() << std::endl << std::endl;
    for (size_t i = 0; i < temp.size(); ++i) {
        out << "+ location ["  << i+1 <<"]" << std::endl
            << "    - path    : "    << temp[i].path << std::endl
            << "    - cgi     : "     << temp[i].fastcgi_pass << std::endl
            << "    - root    : "    << temp[i].root << std::endl
            << "    - expires : " << temp[i].expires << std::endl;
    }
    return out;
}