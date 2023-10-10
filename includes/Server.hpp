#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <map>
#include <vector>
struct Location
{       
        std::string     path;
        std::string     fastcgi_pass;
        std::string     root;
        std::string     expires;
};

class Server {
	private:
                int             number_of_servers;
	        std::vector     <Server> servers;
                std::vector     <Location> locations;
                Location        location;
                std::string     root;
                uint16_t        port;
                std::string     server_name;
	public:
		Server();
		Server(std::string const & filename);
		Server(Server const & src);
		Server & operator=(Server const & obj);
		~Server();
		
		int 								count_servers(std::string const & filename);
		void								parse_configue(std::string const & filename);
		std::string 						skip_spaces(std::string line);
       // void                              parse_server(std::string line, Server tmp);
       
        std::string get_root();
        // std::string get_location();
        std::string get_server_name();
        uint16_t    get_port();

        void set_root(std::string value);
        // void set_location(std::string value);
        void set_server_name(std::string value);
        void set_port(uint16_t value);

};
std::ostream&   operator<<( std::ostream& out,  Server& obj );
#endif