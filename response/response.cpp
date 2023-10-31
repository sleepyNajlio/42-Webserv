#include "response.hpp"
#include "../Multiplexing/inc/Client.hpp"
#include "../Conf/incs/conf_parser.hpp"


Response::Response() 
{
    this->status_code = 0;
    this->initStatusCodeMap();
}

void Response::set_status_code(int status_code)
{
    this->status_code = status_code;
}

int Response::get_status_code() const
{
    return this->status_code;
}

Response::~Response() {}

void Response::initStatusCodeMap()
{
    status_code_map[400] = "Bad Request";
    status_code_map[414] = "Request-URI Too Large";
    status_code_map[505] = "HTTP Version Not Supported";
    status_code_map[501] = "Not Implemented";
    status_code_map[411] = "Length Required";
    status_code_map[405] = "Method Not Allowed";
    status_code_map[404] = "Not Found";
    status_code_map[403] = "Forbidden";
    status_code_map[301] = "Moved Permanently";
    status_code_map[302] = "Found";
    status_code_map[204] = "No Content";
    status_code_map[201] = "Created";
    status_code_map[200] = "OK";
    status_code_map[500] = "Internal Server Error";
    status_code_map[413] = "Payload Too Large";
    status_code_map[409] = "Conflict";
    status_code_map[408] = "Request Timeout";
    status_code_map[512] = "Bad Gateway";
    status_code_map[503] = "Service Unavailable";
}

void Response::init_response(std::pair < Client, Server_storage > client)
{
    if (status_code != 0)
    {
        errPage();
    }
    else
    {
        try {
            // location match
            // check if method is allowed
            if (client.first.req.getMethod() == "GET")
                // handle Get
            else if (client.first.req.getMethod() == "Post")
                // handle Post
            else
                // handle other methods (delete)
        }
        catch(std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }

    }
}

            
// void   ft_response(Client client, Server_storage Serv)
// {
//     try {
//         if (Serv.getLocations().size() == 0)
//         return ;

//         // first step
//         // match locations ;
//         std::vector<Location_storage> locations = Serv.getLocations();
//         Request request = client.get_request();

//         std::string path = request.getUrl();
//         for (size_t i = 0; i < locations.size(); i++)
//         {
//             if (locations[i].getLocaPath() == path)
//             {
//                 std::cout << "location found" << path << std::endl;
//                 break ;
//             }
//         }
//         return ;}
//         catch (std::exception &e)
//         {
//             std::cout << e.what() << std::endl;
//         }
//             // first : if url match any locations

//         // second : keep removing slash until matching url with location path  /kapouet/pouic/toto/pouet
        
//         // exp: /kapouet is rooted to /tmp/www, url /kapouet is
//        // /tmp/www/pouic/toto/pouet

//         // check which method or if any 

//         // check if autoindex is on or off 
//             //if on : responde with indexes incide the location 
//             //else : index not found 404

//             // if off : check if the url is dir or file
//             //if dir : list all files 
//             //if file : respond with file content

// }