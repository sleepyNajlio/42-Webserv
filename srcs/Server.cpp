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

// Server Server::parse_server(std::string line)
// {}
void Server::parse_configue(std::string const & filename) {
    
    std::ifstream file(filename.c_str());
    std::string line;
	std::string key;
    std::string value;
    Server tmp;
    int i = 0;
	
    
    this->servers[this->number_of_servers];
   // (void) number_of_servers;
    if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");
    while (std::getline(file, line)) {
            if(line.find("server") != std::string::npos)
            { 
               while(std::getline(file, line) && line.find('}')  == std::string::npos )
               {
                    //parse_server(line);
                    std::istringstream temp(skip_spaces(line));
                    std::getline(temp, key,' ');
            	    temp >> value;
                    if(!skip_spaces(line).empty())
                    {    
                        if(key == "server_name")
                           tmp.set_server_name(value);
                        if(key == "root")
                            tmp.set_root(value);
                        if(key == "port")
                            tmp.set_port(stoi(value));
                         if(key == "location")
                         {  
                            tmp.location.path = value;

                         }  
                    } 
                    if(line.find("location") != std::string::npos)
                    {
                        std::cout << tmp.location.root << std::endl;
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
// std::string Server::get_location(){
//     return this->location;
// }
std::string Server::get_server_name(){
    return this->server_name;
}
uint16_t    Server::get_port(){
    return this->port;
}

void        Server::set_root(std::string value){
    this->root = value;
}
// void        Server::set_location(std::string value){
//     this->location = value;
// }
void        Server::set_server_name(std::string value){
    this->server_name = value;
}
void        Server::set_port(uint16_t value){
    this->port = value;
}

std::ostream&   operator<<( std::ostream& out,  Server& obj ) {
    out << "------------- Server -------------" << std::endl;
    out << "server_name : " << obj.get_server_name() << std::endl
      << "port : " << obj.get_root() << std::endl
      << "root : " << obj.get_root() << std::endl;
     /// << "location : " << obj.get_location() << std::endl;

    return out;
}