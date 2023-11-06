#include "response.hpp"


Response::Response() : status_code(0), content_lenght(0), clear_client(false), check_res(false) {

        response = "";
        head = "";
        body = "";
        fd_res_filename = "";
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
          this->fd_res_filename = path;
        std::cout << "path = " << path << std::endl;
        fd_res.open(path, std::ios::in | std::ios::binary | std::ios::ate);
        fd_res.seekg(0, std::ios::end);
        content_length = fd_res.tellg();
        fd_res.seekg(0, std::ios::beg);
        if (fd_res.is_open())
		{
            
            this->head = "HTTP/1.1 " + std::to_string(code) +  "\r\nContent-Type: text/html\r\nContent-Length: " 
            + std::to_string(content_length) + "\r\n\r\n" ;
            this->response = this->head;
          //  send(fd_sok, this->head.c_str() ,this->head.size(), 0);

        }
        else
            generateErrorPage(code);
		return;
	}
    else
        generateErrorPage(302);

    // if the error page is not found we should generate one
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
        this->response = header + output;  
		// if (send(fd_sok, header.c_str(), header.size(), 0) <= 0)
		// 	return;
		// if (send(fd_sok, output.c_str(), output.length(), 0) <= 0)
		// 	return;
        clear_client = true;
	}
	else
	{
        errPage(server,403);
	}
}

void    Response::open_file(Server_storage &server, std::string file)
{
    size_t size;
    std::string header;
    this->fd_res_filename = file;
        fd_res.open(file, std::ios::in | std::ios::binary | std::ios::ate);
		fd_res.seekg(0, std::ios::end);
		size = fd_res.tellg();
        std::cout << "----->size = " << size << std::endl;
		fd_res.seekg(0, std::ios::beg);
     //   contentTrack = size;
		if (!fd_res.is_open())
        {
            errPage(server, 403);
            return;
        }
		this->head = "HTTP/1.1 200 OK\r\n"
						"Connection: close\r\n"
						"Content-Type: " +
						get_content_type(file) + "\r\n"
												 "Content-Length: " +
						ft_to_string(size) + "\r\n\r\n";
          contentTrack = size + this->head.size();
        // if (send(fd_sok, header.c_str(), header.size(), 0) <= 0)
		// {
		// 	return;
		// }
}

void Response::ft_sendHeader()
{
    bytes_sent += send(fd_sok, this->head.c_str(), this->head.size(), 0);
    this->head = "";
    if (bytes_sent <= 0 )
    {
        std::cout << "error send" << std::endl;
        clear_client = true;
        return;
    }
}
void Response::ft_sendResponse()
{
     if(this->head.size() != 0)
        ft_sendHeader();
    
    char buffer[2048];
    fd_res.read(buffer, 2048);
    size_t buffer_size = fd_res.gcount();

    if (buffer_size)
    {
        std::cout << "---------->>>>>>bytes_sent = " << bytes_sent << "----contentTrack = " << contentTrack  << std::endl;
        if(this->response.size() != 0)
        {
            bytes_sent += send(fd_sok, this->response.c_str(), this->response.size(), 0);
            if (bytes_sent <= 0 )
            {
                std::cout << "error send" << std::endl;
                clear_client = true;
                return;
            }
            this->response = "";
        }
        else
        {
            bytes_sent += send(fd_sok, buffer, fd_res.gcount(), 0);
            if (bytes_sent <= 0 )
            {
                std::cout << "error send" << std::endl;
                clear_client = true;
                return;
            }
        }
        bzero((buffer), 2048);
    }
    if (bytes_sent == contentTrack)
    {
        std::cout << "end of file" << std::endl;
        clear_client = true;
    }

}
std::string get_ex(std::string path)
{
        std::istringstream temp(path);
		std::string name;
        std::string ex;

		std::getline(temp, name, '.');
		temp >> ex;
        return ex;
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
        std::cout << "file :::"<< file << std::endl;
        std::ifstream file1(file);

        //-------->CGI HANDLER
		if(get_ex(request.getUrl()) == "py" || get_ex(request.getUrl()) == "php")
            Cgi cgi(request , file);
        else if (file1.good())
        {
			open_file(server,  file);
            std::cout << "file found"<< file << std::endl;
        }
		else if (access(file.c_str(), F_OK))
        {
            std::cout << "file not found"<< file << std::endl;
			errPage(server,404);
        }
		else
        {
            std::cout << "file forbiden"<< file << std::endl;
            errPage(server,403);
        }
    }
}

void	Response::ft_delete(Request &request,Server_storage &server )
{
    std::string path = "." + request.getUrl();
	if (access(path.c_str(), W_OK) == -1)
		errPage(server,403);
    else if (std::remove(path.c_str()))
        errPage(server,403);
}

void Response::ft_Post(Request &request)
{
        std::string name = "./uploads" + request.getUrl();
        if (std::rename(request.getRandomStr().c_str(), name.c_str()) != 0) {
        std::perror("Error renaming file");
        }
}

void   Response::init_response(Request &request , Server_storage &server)
{
   
    // std::cout << "status code = "<<get_status_code() << std::endl;
  //  if (get_status_code())
        // errPage(server,0);
    // if (check_res == false){
    //     check_res = true;
        locIt = locationMatch(server, request.getUrl());
    // }
    // if (locIt->getLocaPath() != "")
    //     std::cout << "------>> "<< locIt->getLocaPath() << std::endl;
    storage_int allowedMethods = locIt->getLocaAllowedMethods();
    if (allowedMeth(allowedMethods, request.getMethod()))
    {
        std::cout << request.getMethod() << std::endl;
        if (request.getMethod() == "GET")
            ft_Get(request, server);
        else if (request.getMethod() == "POST")
            ft_Post(request);
        else if (request.getMethod() == "DELETE")
            ft_delete(request , server);
    }
    else
        std::cout << " method not allowed " << std::endl;
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
