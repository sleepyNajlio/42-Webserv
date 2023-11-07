#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>

#include "../request/request.hpp"


class Cgi
{
    private :
        Request                             &req;
		const std::string 					path;
        std::string                         name;
        std::map<std::string, std::string>  env;
        int                                 status;

    public :
        std::string                         response;
        Cgi(Request &req, const std::string path);
//         int                 execute_cgi(std::string filename);
        char                **getEnv();
        char                **env_to_char (std::map<std::string, std::string>& env);
        int                 execute_cgi(std::string filename);
       const std::string&  getCgiResponse() const;
};

#endif