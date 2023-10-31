#include "response.hpp"


Response::Response() 
{
    this->status_code = 0;
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


std::string Response::set_head()
{
	std::string head = "";
	
    head += "HTTP/1.1 404 KO\r\n Content-Type: text/html\r\n";        
	head += "Content-Length: " + std::to_string(this->content_length) + "\r\n";
	head += "Cache-Control: no-cache\r\n";
	head += "Accept: */*\r\n";
	head += "Date: dima \r\n";
	head += "Accept-Encoding: gzip, deflate, br\r\n";	
	return (head);
}
void Response::errPage(Server_storage server ,int code, int fd)
{
    const std::map<int, std::string>& errors = server.getErrorPages();
	std::string name;
    std::string head;
     for (const auto& pair : errors) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
    std::ifstream 			file;


  //  std::cout << errors.find(code)<< std::endl
    if (errors.find(code) != errors.end())
	{
		name = errors.find(code)->second;
		file.open(name, std::ifstream::binary | std::ifstream::ate);
		std::cout << name << std::endl;
        if (file.is_open())
		{
			this->content_length = file.tellg();
			file.seekg(0, std::ios::beg);
			std::vector<char> vec((int)this->content_length);
			file.read(&vec[0], (int)this->content_length);
			std::string response(vec.begin(), vec.end());
   
			std::string head= "HTTP/1.1 404 \r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(this->content_length) + "\r\n\r\n" + response;
	        this->response = head;
             if (send(fd + 1 , head.c_str(), head.size(), 0) < 1)

            this->response =  response;
		}
      if (send(fd + 1 , this->response.c_str(), response.size(), 0) < 1)
        {
            std::cout << "Error sending response header" << std::endl;
            return;
        }
      file.close();
		return;
	}
            std::cout << "bey" << std::endl;
            std::cout << response<< std::endl;
}

void   Response::init_response(Request request , Server_storage server)
{
    (void) request;
    (void) server;
    std::cout << "status code = "<<get_status_code() << std::endl;
  //  if (get_status_code())
        errPage(server,404,server.getFd());
            std::cout << "response = " << response<<  std::endl;
        std::cout << server.getFd()<< std::endl;
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