#include "../includes/Server.hpp"

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
        // std::cout << "------------- Server " << i + 1 << " -------------" << std::endl;
        // std::cout << tmp << std::endl;
        i++;
        }
    }
	file.close();
}
