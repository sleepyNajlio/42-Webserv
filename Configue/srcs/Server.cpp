#include "../includes/Server.hpp"

Server::Server() {}

Server::~Server() {}

Server::Server(std::string const & filename) {
    check_syntax(filename);
        this->number_of_servers = count_servers(filename);
        this->parse_configue(filename);    
}

Server::Server(Server const & src) {
    *this = src;
}

Server & Server::operator=(Server const & obj) {
    if (this != &obj) {
        this->server_name = obj.server_name;
        this->root = obj.root;
        this->location = obj.location;
        this->port = obj.port; 
    }
    return *this;
}

int         Server::get_number_of_servers()
{
    return this->number_of_servers;
}

std::vector <Server> Server::get_servers()
{
    return this->servers;
}

std::string Server::get_root(){
    return this->root;
}

std::string Server::get_server_name(){
    return this->server_name;
}

int    Server::get_port(){
    return this->port;
}

std::string Server::get_path(){
    return this->location.path;
}

std::string Server::get_fastcgi_pass(){
    return this->location.fastcgi_pass;
}

std::string Server::get_location_root(){
    return this->location.root;
}

std::string Server::get_expires(){
    return this->location.expires;
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
