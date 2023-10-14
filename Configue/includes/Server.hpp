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
	        std::vector     < Server >      servers;
                std::vector     < Location >    locations;
              //  std::vector     < Client >      clients;
                Location        location;
                std::string     root;
                int             port;
                std::string     server_name;
	public:
		Server();
		Server(std::string const & filename);
		Server(Server const & src);
		Server & operator=(Server const & obj);
		~Server();
		
		int 		        count_servers(std::string const & filename);
                void		        parse_configue(std::string const & filename);
		std::string 		skip_spaces(std::string line);
		void                    check_syntax(std::string const &filename);
                void                    parse_location(std::string line,Location& test);
                void                    parse_server(std::string key, std::string value, Server &tmp);
                void                    check_brackets(std::string const & filename);

                //void                              parse_server(std::string line, Server tmp);
       
        std::string get_root();
        int         get_number_of_servers();
        std::vector <Server> get_servers();
        // std::string get_location();
        std::string get_server_name();
        int         get_port();
        std::string get_path();
        std::string get_fastcgi_pass();
        std::string get_location_root();
        std::string get_expires();
        std::vector <Location> get_locations();


        void set_root(std::string value);
        // void set_location(std::string value);
        void set_server_name(std::string value);
        void set_port(int value);
        void get_path(std::string value);
        // void get_fastcgi_pass(std::string value);
        // void get_root(std::string value);
        // void get_expires(std::string value);
};
std::ostream&   operator<<( std::ostream& out,  Server& obj );
#endif