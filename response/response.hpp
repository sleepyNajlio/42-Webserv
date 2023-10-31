#pragma once

#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include <utility>

// #include "../Multiplexing/inc/Client.hpp"
// #include "../Conf/incs/conf_parser.hpp"

class Client;
class Server_storage;

class Response {
    private:
            int status_code;
    public:
            Response();
            ~Response();
            void    set_status_code(int status_code);
            int     get_status_code() const;
            std::map<int, std::string> status_code_map;
            void    initStatusCodeMap();
            void    writeResponse();
            void    errPage();
            void   init_response(std::pair < Client, Server_storage > client);
};



