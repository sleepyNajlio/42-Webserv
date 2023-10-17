#ifndef PARSER_HPP
#define PARSER_HPP

#include "Server.hpp"


class Parser
{     
    private:
         int             number_of_servers;
         std::vector     <Server> servers;
    public:  
		Parser(std::string const & filename);

        int                 get_number_of_servers();
        std::vector <Server>    get_servers();
        int 		        count_servers(std::string const & filename);
		void		        parse_configue(std::string const & filename);
		std::string 		skip_spaces(std::string line);
        void                parse_location(std::string line,Location& test);
        void                parse_server(std::string key, std::string value, Server &tmp);
        void                check_syntax(std::string const & filename);
        void                check_brackets(std::string const & filename);
     
};

#endif