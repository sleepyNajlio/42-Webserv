#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "Location.hpp"



class Server {
	private: 
              //  Location        location;
                std::string     root;
                int             port;
                std::string     server_name;
                std::vector     <Location> locations;
	public:
		Server();
		Server(Server const & src);
		Server & operator=(Server const & obj);
		~Server();
		
		       
                std::string get_root();
                std::string get_server_name();
                int         get_port();
                std::string get_path(int i);
                std::string get_fastcgi_pass(int i);
                std::string get_location_root(int i);
                std::string get_expires(int i);
                std::vector <Location> get_locations();
                void        set_locations(Location value);


                 void set_root(std::string value);
                // void set_location(std::string value);
                 void set_server_name(std::string value);
                 void set_port(int value);
                // void set_locations(Location value);
                // void get_path(std::string value);
                // void get_fastcgi_pass(std::string value);
                // void get_root(std::string value);
                // void get_expires(std::string value);
};
std::ostream&   operator<<( std::ostream& out,  Server& obj );
#endif