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

std::string  Response::get_response()
{
    return this->response;
}

void Response::set_response(std::string data)
{
    this->response = data ;
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

void Response::generateErrorPage(int code)
{
    std::string errorMessage = initStatusCodeMap(code);
    std::string errorPageBody = "<!DOCTYPE html>\n";
    errorPageBody += "<html>\n";
    errorPageBody += "<head>\n";
    errorPageBody += "<title>Error " + std::to_string(code) + ": " + errorMessage + "</title>\n";
    errorPageBody += "<style>\n";
    errorPageBody += "body {display: flex; justify-content: center; align-items:center; flex-direction: column; font-family: Arial, sans-serif; margin: 0; padding: 20px;}\n";
    errorPageBody += "h1 {font-size: 24px;}\n";
    errorPageBody += "p {font-size: 16px;}\n";
    errorPageBody += "</style>\n";
    errorPageBody += "</head>\n";
    errorPageBody += "<body>\n";
    errorPageBody += "<h1>Error " + std::to_string(code) + ": " + errorMessage + "</h1>\n";
    errorPageBody += "<p>" + std::to_string(code) + "</p>\n";
    errorPageBody += "</body>\n";
    errorPageBody += "</html>";
    this->response = "HTTP/1.1 " + std::to_string(code) +  "\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(errorPageBody.size()) + "\r\n\r\n" + errorPageBody;
}


void Response::errPage(Server_storage server ,int code)
{
	std::string                         path;
    std::string                         head;
    std::ifstream                       file;
    std::ifstream::pos_type             content_length;
    const std::map<int, std::string>&   errors = server.getErrorPages();

    if (errors.find(code) != errors.end())
	{
        path = errors.find(code)->second;
		file.open(path, std::ifstream::binary | std::ifstream::ate);
        if (file.is_open())
		{
			content_length = file.tellg();
			file.seekg(0, std::ios::beg);
			std::vector<char> vec((int)content_length);
			file.read(&vec[0], (int)content_length);
			std::string body(vec.begin(), vec.end());
            
            this->response = "HTTP/1.1 " + std::to_string(code) +  "\r\nContent-Type: text/html\r\nContent-Length: " 
            + std::to_string(content_length) + "\r\n\r\n" + body;

           // this->response = head + body; 
            // if (send(client_fd, head.c_str(), head.size(), 0) < 1)
            //     std::cout << "Error sending body header" << std::endl;
    
            // if (send(client_fd, body.c_str(), body.size(), 0) < 1)
            // {
            //     std::cout << "Error sending body header" << std::endl;
            //     file.close();
            //     return;
            // }
        }
        else
            generateErrorPage(code);
            file.close();
		return;
	}
    else
        generateErrorPage(302);

    // if the error page is not found we should generate one
}

static std::string newpath(std::string path)
{
	size_t pos = -1;
	while (path.find("/", pos + 1) != std::string::npos)
		pos = path.find("/", pos + 1);
	path = path.substr(0, pos);
	if (path.empty())
		return ("/");
	return (path);
}

static  std::vector<Location_storage>::const_iterator locationMatch(Server_storage &serv, std::string path)
{
	std::vector<Location_storage>::const_iterator it;
	while (1)
	{
		it = serv.getLocations().begin();
		while (it != serv.getLocations().end())
		{
			if (it->getLocaPath() == path)
            {
				return (it);
            }
			it++;
		}
		if (path == "/")
		{	
            break;
        }
		path = newpath(path);
	}

	return (it);
}

static std::string delRepSlash(std::string file)
{
	int p;
	while (1)
	{
		p = file.find("//");
		if (p != -1)
			file = file.substr(0, p) + file.substr(p + 1, file.length());
		else
			break;
	}
	return (file);
}


bool allowedMeth(storage_int& allowedMethods, std::string method)
{

    if (allowedMethods[0] == 1 && method == "GET")
        return (true);
    else if (allowedMethods[1] == 1 && method == "POST")
        return (true);
    else if (allowedMethods[2] == 1 && method == "DELETE")
        return (true);
    return (false);
}

bool isDir(std::string path)
{
	struct stat buffer;
	if (stat(path.c_str(), &buffer) == 0)
	{
		if (S_ISDIR(buffer.st_mode))
		{
			// std::cout << path << " is a directory" << std::endl;
			return (1);
		}
	}
	return (0);
}

void Response::listDir(std::string file, Request &request, Server_storage &server)
{
	std::string output;
	output.append("<html><body><ul>");

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(file.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			output.append("<li><a href=\"");
			if (locIt->getLocaPath() == "/" && request.getUrl() == "/")
				output.append(request.getUrl() + ent->d_name);
			else
				output.append(request.getUrl() + "/" + ent->d_name);
			output.append("\">");
			output.append(ent->d_name);
			output.append("</a></li>");
		}
		closedir(dir);
	output.append("</ul></body></html>");
	std::string header = "HTTP/1.1 200 OK\r\n"
					"Connection: close\r\n"
					"Content-Type: "
					"text/html\r\n"
					"Content-Length: " +
					ft_to_string(output.size()) +
					"\r\n\r\n";
		if (send(fd_sok, header.c_str(), header.size(), 0) <= 0)
			return;
		if (send(fd_sok, output.c_str(), output.length(), 0) <= 0)
			return;
	}
	else
	{
        errPage(server,403);
	}
}

void    Response::ft_Get(Request &request, Server_storage &server)
{
    std::string file;
    (void) server;
	file = request.getUrl();
	// size_t i = 0;
	if (locIt->getLocaPath() != "/")
		file.replace(0, locIt->getLocaPath().length(), locIt->getLocaRoot());
	else
		file.replace(0, locIt->getLocaPath().length() - 1, locIt->getLocaRoot());
    file = delRepSlash(file);
    if (isDir(file))
    {
        if (locIt->getLocaAutoindex())
        {
            listDir(file, request, server);
        }
        else
        {
            std::cout << "index not found" << std::endl;
        }
    }
    else
    {
        std::cout << "file" << std::endl;
       	// std::ifstream file1(file);
		// if (!locIt->cgi_path.empty() && check_ext(file))
		// {
		// 	//exec_cgi(client, file);
		// }
		// else 
        // if (file1.good())
		// 	open_file(client, file);
		// else if (access(file.c_str(), F_OK))
		// 	errPage(server,404);
            //statut_code(client, "404", "404 Not Found");
		// else
        //     errPage(server,403);
			//statut_code(client,  "403", "403 Forbidden");
    }
}

// void    open_file()
// {
    
// }

void   Response::init_response(Request &request , Server_storage &server)
{
    // (void) request;
    // (void) server;
    // std::cout << "status code = "<<get_status_code() << std::endl;
  //  if (get_status_code())
        // errPage(server,0);
    locIt = locationMatch(server, request.getUrl());
    if (locIt->getLocaPath() != "")
        std::cout << locIt->getLocaPath() << std::endl;
    storage_int allowedMethods = locIt->getLocaAllowedMethods();
    if (allowedMeth(allowedMethods, request.getMethod()))
    {
        if (request.getMethod() == "GET")
        {
            ft_Get(request, server);
            // handle Get
        }
        else if (request.getMethod() == "POST")
        {
            std::cout << "POST" << std::endl;
            // handle Post
        }
        else if (request.getMethod() == "DELETE")
        {
            std::cout << "DELETE" << std::endl;
            // handle Delete
        }
    }
    else
        std::cout << "method not allowed" << std::endl;
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