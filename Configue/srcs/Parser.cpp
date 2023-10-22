#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"

Parser::Parser(std::string const & filename) {
    this->number_of_servers = count_servers(filename);
    parse_configue(filename);
    check_syntax(filename);    
}

std::string Parser::skip_spaces(std::string line)
{
    int start = 0;
    while(line[start] && std::isspace(line[start])) {start++;}
    return line.substr(start, line.length());
}

int Parser::count_servers(std::string const & filename)
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


void Parser::parse_location(std::string line,Location& test)
{
    std::string key;
    std::string value;
    std::istringstream temp(skip_spaces(line));
    std::getline(temp, key,' ');
    temp >> value;
    if(key == "fastcgi_pass")
        test.set_fastcgi_pass(value.substr(0,value.length() - 1)) ;
    if(key == "root")
        test.set_location_root(value.substr(0,value.length() - 1));
    if(key == "expires")
        test.set_expires(value.substr(0,value.length() - 1)); 
}

void Parser::parse_server(std::string key, std::string value, Server &tmp)
{
    if(key == "listen")
        tmp.set_port(stoi(value.substr(0,value.length() - 1)));
    if(key == "server_name")
        tmp.set_server_name(value.substr(0,value.length() - 1));
    if(key == "root")
        tmp.set_root(value.substr(0,value.length() - 1));
}
void Parser::parse_configue(std::string const & filename) {
    
    std::ifstream file(filename.c_str());
    std::string line;
	std::string key;
    std::string value;
  
	
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
                     test.set_path(value);
                     while(std::getline(file, line) && line.find('}')  == std::string::npos ){ 
                         parse_location(line,test);
                     }
                     tmp.set_locations(test);
                  }  
                 parse_server(key, value,tmp);
             }     
        }
        servers.push_back(tmp);
        }
    }
	file.close();
}

int         Parser::get_number_of_servers()
{
    return this->number_of_servers;
}

std::vector <Server> Parser::get_servers()
{
    return this->servers;
}

std::ostream&   operator<<( std::ostream& out, std::vector  <Server> servers){
    
    for (size_t i = 0; i < servers.size(); ++i) {
        std::cout << "------------- Server " << i + 1 << " -------------" << std::endl;
        std::cout << servers[i] << std::endl;
    }
    return out;
}