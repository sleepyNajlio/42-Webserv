#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Server.hpp"

class Location
{     
    public :  
        std::string     path;
        std::string     fastcgi_pass;
        std::string     root;
        std::string     expires;
    
        void set_server_name(std::string value);
        void set_locations(Location value);
        void set_port(int value);
        void get_path(std::string value);

};

#endif