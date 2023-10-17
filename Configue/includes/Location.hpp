#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Server.hpp"

class Location
{     
    private:  
        std::string     path;
        std::string     fastcgi_pass;
        std::string     root;
        std::string     expires;
    public:

        std::string get_path();
        std::string get_fastcgi_pass();
        std::string get_location_root();
        std::string get_expires();
        void        set_path(std::string value);
        void        set_fastcgi_pass(std::string value);
        void        set_location_root(std::string value);
        void        set_expires(std::string value);  
};

#endif