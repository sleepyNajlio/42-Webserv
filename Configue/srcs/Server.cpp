#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"

Server::Server() {}

Server::Server(Server const & src) {
    *this = src;
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


int    Server::get_port(){
    return this->port;
}

std::string Server::get_root(){
    return this->root;
}

std::string Server::get_server_name(){
    return this->server_name;
}

void        Server::set_port(int value){
        this->port = value;
}

void        Server::set_root(std::string value){
    this->root = value;
}

void        Server::set_server_name(std::string value){
    this->server_name = value;
}

void Server::set_locations(Location value)
{
    locations.push_back(value);
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
        out << "+ location ["       << i+1 << "]" << std::endl
            << "    - path    : "   << temp[i].get_path() << std::endl
            << "    - cgi     : "   << temp[i].get_fastcgi_pass() << std::endl
            << "    - root    : "   << temp[i].get_location_root() << std::endl
            << "    - expires : "   << temp[i].get_expires() << std::endl;
    }
    return out;
}