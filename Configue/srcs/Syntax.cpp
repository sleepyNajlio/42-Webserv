#include "../includes/Server.hpp"

void    Server::check_brackets(std::string const & filename)
{
     std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");
	
    std::string line;
	int brackets = 0;
    while (std::getline(file, line)) {
        line  = skip_spaces(line);
            if(line.empty())
                continue ;
            else if(line.find("{") != std::string::npos)
                brackets++;
            else if(line.find("}") != std::string::npos)
                brackets--;
    }   
    if(brackets > 0)
        throw std::runtime_error("Error: expected }");
    else if(brackets < 0) 
        throw std::runtime_error("Error: expected {");
}

void Server::check_syntax(std::string const & filename)
{
    check_brackets(filename);
    std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");
    std::string line;
    int i = 0;
    while (std::getline(file, line)) {
        i++;
            line  = skip_spaces(line);
            if(line.empty() || line.find("{") != std::string::npos ||  line.find("}") != std::string::npos)
                continue ;
            else if(line.find(";") != std::string::npos)
                continue;
            else 
            {
                std::cout <<"Error: in line " <<  i ;
                throw std::runtime_error(" messing ';'");
            }    
    }
}   
