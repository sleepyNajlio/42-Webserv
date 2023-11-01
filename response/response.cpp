#include "response.hpp"


Response::Response() 
{
    this->status_code = 0;
    this->client_fd = 0;
   // this->initStatusCodeMap(code);
}

void Response::set_status_code(int status_code)
{
    this->status_code = status_code;
}

int Response::get_status_code() const
{
    return this->status_code;
}

void Response::set_client_fd(int client_fd)
{
    this->client_fd = client_fd;
}

int Response::get_client_fd() const
{
    return this->client_fd;
}

Response::~Response() {}

std::string Response::initStatusCodeMap(int code)
{
    std::map<int, std::string> map;
    map[400] = "Bad Request";
    map[414] = "Request-URI Too Large";
    map[505] = "HTTP Version Not Supported";
    map[501] = "Not Implemented";
    map[411] = "Length Required";
    map[405] = "Method Not Allowed";
    map[404] = "Not Found";
    map[403] = "Forbidden";
    map[301] = "Moved Permanently";
    map[302] = "Found";
    map[204] = "No Content";
    map[201] = "Created";
    map[200] = "OK";
    map[500] = "Internal Server Error";
    map[413] = "Payload Too Large";
    map[409] = "Conflict";
    map[408] = "Request Timeout";
    map[512] = "Bad Gateway";
    map[503] = "Service Unavailable";

    return map.find(code)->second;
}


void Response::errPage(Server_storage server ,int code)
{
	std::string                         path;
    std::string                         head;
    std::ifstream                       file;
    const std::map<int, std::string>&   errors = server.getErrorPages();

    if (errors.find(code) != errors.end())
	{
        path = errors.find(code)->second;
		file.open(path, std::ifstream::binary | std::ifstream::ate);
        if (file.is_open())
		{
			this->content_length = file.tellg();
			file.seekg(0, std::ios::beg);
			std::vector<char> vec((int)this->content_length);
			file.read(&vec[0], (int)this->content_length);
			std::string response(vec.begin(), vec.end());
			head = "HTTP/1.1 " + std::to_string(code) +  "\r\nContent-Type: text/html\r\nContent-Length: " 
            + std::to_string(this->content_length) + "\r\n\r\n" + response;

            if (send(client_fd, head.c_str(), head.size(), 0) < 1)
                std::cout << "Error sending response header" << std::endl;
    
            if (send(client_fd, response.c_str(), response.size(), 0) < 1)
            {
                std::cout << "Error sending response header" << std::endl;
                file.close();
                return;
            }
        }
            file.close();
		return;
	}
    // if the error page is not found we should generate one
}

void   Response::init_response(Request request , Server_storage server)
{
    (void) request;
    (void) server;
    // std::cout << "status code = "<<get_status_code() << std::endl;
  //  if (get_status_code())
  std::cout << "fd " << client_fd << std::endl;
        errPage(server,404);
    // else
    // {
    //     try {
    //         // location match
    //         // check if method is allowed
    //         if (client.first.req.getMethod() == "GET")
    //             // handle Get
    //         else if (client.first.req.getMethod() == "Post")
    //             // handle Post
    //         else
    //             // handle other methods (delete)
    //     }
    //     catch(std::exception &e)
    //     {
    //         std::cout << e.what() << std::endl;
    //     }

    // }
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