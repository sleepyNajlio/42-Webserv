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

void Server::parse_location(std::string line,Location& test)
{
    std::string key;
    std::string value;
    std::istringstream temp(skip_spaces(line));
    std::getline(temp, key,' ');
    temp >> value;
    if(key == "fastcgi_pass")
        test.fastcgi_pass = value.substr(0,value.length() - 1);
    if(key == "root")
        test.root = value.substr(0,value.length() - 1);
    if(key == "expires")
        test.expires = value.substr(0,value.length() - 1); 
}

void Server::parse_server(std::string key, std::string value, Server &tmp)
{
    if(key == "listen")
        tmp.set_port(stoi(value.substr(0,value.length() - 1)));
    if(key == "server_name")
        tmp.set_server_name(value.substr(0,value.length() - 1));
    if(key == "root")
        tmp.set_root(value.substr(0,value.length() - 1));
}
void Server::parse_configue(std::string const & filename) {
    
    std::ifstream file(filename.c_str());
    std::string line;
	std::string key;
    std::string value;
    int i = 0;
	
    this->servers[this->number_of_servers];
    if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");
    
    while (std::getline(file, line))
    {
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
                  if(key == "location")
                  {
                    Location test;
                     test.path = value;
                     while(std::getline(file, line) && line.find('}')  == std::string::npos ){ 
                         parse_location(line,test);
                     }
                     tmp.locations.push_back(test);
                  }  
                 parse_server(key, value,tmp);
             }     
        }
        servers.push_back(tmp);
        std::cout << "------------- Server " << i + 1 << " -------------" << std::endl;
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