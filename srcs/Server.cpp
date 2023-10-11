#include "../includes/Server.hpp"

Server::Server() {}

Server::Server(std::string const & filename) {
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

Server::~Server() {}

std::string Server::skip_spaces(std::string line)
{
    int start = 0;
    while(line[start] && std::isspace(line[start])) {start++;}
    return line.substr(start, line.length());
}

int Server::count_servers(std::string const & filename)
{
    std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");
	
    std::string line;
	int i = 0;
    while (std::getline(file, line)) {
            if(line.find("server") != std::string::npos)
            { 
               while(std::getline(file, line) && line.find('}') == std::string::npos){continue;}
                i++;
            }
    }
	return i;
}

void Server::parse_location(std::string line,Server &tmp)
{
    std::string key;
    std::string value;
    std::istringstream temp(skip_spaces(line));
    std::getline(temp, key,' ');
    temp >> value;
    if(key == "fastcgi_pass")
        tmp.location.fastcgi_pass = value;
    if(key == "root")
        tmp.location.root = value;
    if(key == "expires")
        tmp.location.expires = value; 
}

// void Server::parse_server(std::string line,Server &tmp)

void Server::parse_configue(std::string const & filename) {
    
    std::ifstream file(filename.c_str());
    std::string line;
	std::string key;
    std::string value;

    int i = 0;
	
    
    this->servers[this->number_of_servers];
    if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");
    while (std::getline(file, line)) {
         Server tmp;
            if(line.find("server") != std::string::npos)
            { 
               while(std::getline(file, line) && line.find('}')  == std::string::npos )
               {
                    std::istringstream temp(skip_spaces(line));
                    std::getline(temp, key,' ');
            	    temp >> value;
                    if(!skip_spaces(line).empty())
                    {    
                        if(key == "listen")
                            tmp.set_port(stoi(value));
                        if(key == "server_name")
                            tmp.set_server_name(value);
                        if(key == "root")
                            tmp.set_root(value);
                         if(key == "location")
                         {
                            tmp.location.path = value;
                            while(std::getline(file, line) && line.find('}')  == std::string::npos ){ 
                                parse_location(line,tmp);
                            }
                         }  
                    } 
            
               }
                servers.push_back(tmp);
                std::cout << tmp << std::endl;
                i++;
            }
    }
	file.close();
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

std::ostream&   operator<<( std::ostream& out,  Server& obj ) {
    out << "------------- Server -------------" << std::endl;
    out << "port : " << obj.get_port() << std::endl
        << "server_name : " << obj.get_server_name() << std::endl
        << "root : " << obj.get_root() << std::endl;
        
    out << "--------------- location -----------" << std::endl
        << "path :"<<obj.get_path() << std::endl
        << "cgi :" << obj.get_fastcgi_pass() << std::endl
        << "root :" << obj.get_location_root() << std::endl
        << "expires : " << obj.get_expires() << std::endl;

    return out;
}