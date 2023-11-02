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
#include <dirent.h>


class Response {
    private:
        int status_code;
        std::string response;
        std::string head;
        std::string body;
        size_t       content_lenght;


    public:
            Response();
            ~Response();

            void        set_status_code(int status_code);
            void        set_response(std::string data);

            int         get_status_code() const;
            std::string get_response();

            std::vector<Location_storage>::const_iterator locIt;
            int fd_sok;
            int fd_res;
            void        ft_Get(Request &request, Server_storage &server);
            void        listDir(std::string file, Request &request, Server_storage &server);

            std::map<int, std::string> status_code_map;
            void    initStatusCodeMap();
            void    writeResponse();
            void    errPage(Server_storage server,int error_code);
            void    generateErrorPage(int code);
            std::string initStatusCodeMap(int code);
            void   init_response(Request &request , Server_storage &server);
};

template <typename T>
std::string ft_to_string(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

