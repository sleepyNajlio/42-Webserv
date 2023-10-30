#pragma once

#include <sstream>
#include <string>
#include <map>
#include <iostream>

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
};

