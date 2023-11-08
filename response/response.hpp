#pragma once

#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include <fstream>

#include <string.h>
#include <string>
#include "../request/request.hpp"
#include "../Conf/incs/conf_parser.hpp"
#include "../CGI/Cgi.hpp"
#include <dirent.h>
#include <filesystem>

class Cgi;
class Response {   

    public:
        int             status_code;
        std::string     response;
        std::string     head;
        std::string     body;
        size_t          content_lenght;
        std::string     fd_res_filename;  // To store the filename associated with fd_res

    bool                                            clear_client;
    bool                                            check_res;
    int                                             fd_sok;
    ssize_t                                         contentTrack;
    ssize_t                                         bytes_sent;
    std::vector<Location_storage>::const_iterator   locIt;
    std::map<int, std::string>                      status_code_map;
    std::fstream                                    fd_res;
    std::string                                     method;



    Response();
    ~Response();

    // Copy constructor to handle copying the class correctly
    Response(const Response& other) {
        // Perform a deep copy of the class members
        *this = other;
    }

    // Copy assignment operator to handle assignment correctly
    Response& operator=(const Response& other) {
        if (this != &other) {
            check_res = other.check_res;
            clear_client = other.clear_client;
            fd_sok = other.fd_sok;
            // Perform a deep copy of the class members
            status_code = other.status_code;
            response = other.response;
            head = other.head;
            body = other.body;
            content_lenght = other.content_lenght;
            fd_res_filename = other.fd_res_filename;
            fd_res.open(other.fd_res_filename, std::ios::in | std::ios::binary | std::ios::ate);
            fd_res << other.fd_res.rdbuf();
            bytes_sent = other.bytes_sent;
            contentTrack = other.contentTrack;
        }
        return *this;
    }
            void   init_response(Request &request , Server_storage &server);

           //methods
            void        ft_Get(Request &request, Server_storage &server);
            void        ft_Post(Request &request, Server_storage &server);
            void	    ft_delete(Request &request,Server_storage &server );
           
           //senders
            void        ft_sendResponse();
            void        ft_sendHeader();
           
            void        set_status_code(int status_code);
            void        set_response(std::string data);
            
            int         get_status_code() const;
            std::string get_response();

            void        initStatusCodeMap();
            std::string initStatusCodeMap(int code);
            void        errPage(Server_storage server,int error_code);
            void        generateErrorPage(int code);

            void        listDir(std::string file, Request &request, Server_storage &server);
            void        open_file(Server_storage &server, std::string file);
           
};

std::string get_content_type(std::string path);
std::string get_extension(std::string path);
bool        allowedMeth(storage_int& allowedMethods, std::string method);
bool        isDir(std::string path);
std::vector<Location_storage>::const_iterator locationMatch(Server_storage &serv, std::string path);
std::string delRepSlash(std::string file);


template <typename T>
std::string ft_to_string(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


