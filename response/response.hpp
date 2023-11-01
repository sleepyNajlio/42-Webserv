#pragma once

#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include <utility>

#include <string.h>
#include <string>
#include "../request/request.hpp"
#include "../Conf/incs/conf_parser.hpp"



class Response {
    private:
        int client_fd;
        int status_code;
        std::string response;
        std::ifstream::pos_type content_length;

    public:
            Response();
            ~Response();

            void    set_client_fd(int client_fd);
            void    set_status_code(int status_code);

            int     get_client_fd() const;
            int     get_status_code() const;
            std::map<int, std::string> status_code_map;
            void    initStatusCodeMap();
            void    writeResponse();
            void    errPage(Server_storage server,int error_code);
            std::string set_head();
            
                std::string initStatusCodeMap(int code);
            void   init_response(Request request , Server_storage server);
};



