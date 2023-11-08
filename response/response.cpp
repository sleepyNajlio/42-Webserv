#include "response.hpp"


Response::Response() : status_code(0), content_lenght(0), clear_client(false), check_res(false) , contentTrack(0) , bytes_sent(0){

        response = "";
        head    = "";
        body    = "";
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
            // this->response = this->head;
        }
        else
            generateErrorPage(code);
		return;
	}
    else
        generateErrorPage(302);
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
        clear_client = true;
	}
	else
        errPage(server,403);
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
	if (!fd_res.is_open())
    {
        errPage(server, 403);
        return;
    }
	this->head = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: " +
					get_content_type(file) + "\r\nContent-Length: " + ft_to_string(size) + "\r\n\r\n";
    contentTrack = size + this->head.size();
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
        {  Cgi cgi(request , file);
            this->response = cgi.response;
            this->head = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: .py \r\nContent-Length: " + ft_to_string(this->response.size()) + "\r\n\r\n";
        }
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

static bool slashChecker(std::string path)
{
    std::cout << path[path.size() - 1] << std::endl;
    if (path[path.size() - 1] == '/')
        return 1;
    else
        return 0;
}

void Response::ft_Post(Request &request, Server_storage &server)
{
      (void)server;
      std::cout << "***********************" << std::endl;
            slashChecker(request.getUrl());
        if (locIt->loca_upload)
        {
            std::string name = "./uploads" + request.getUrl();
            if (std::rename(request.getRandomStr().c_str(), name.c_str()) != 0) 
            {
                std::perror("Error renaming file");
                head = ""; // 201
            }
        }
        else if (isDir(request.getUrl()))
        {
            slashChecker(request.getUrl());
        }

            /*
            if (slashChecker(server.locations.getUpload()))
            {
                if (server.locations.getlocaIndex())
                {
                    if (get_ex(request.getUrl()) == "py" || get_ex(request.getUrl()) == "php")
                        do it
                    else
                        403
                }
                else
                    error 403
            }
            else
            {
                301 redirect
            }
        }
        else if ()
        {
            if (cgi_path)

        }
        else
            error 404

        */
}

void   Response::init_response(Request &request , Server_storage &server)
{
    locIt = locationMatch(server, request.getUrl());
    storage_int allowedMethods = locIt->getLocaAllowedMethods();
            std::cout << "IM hereeee-------------" << std::cout;
    if (allowedMeth(allowedMethods, request.getMethod()))
    {
        std::cout << request.getMethod() << std::endl;
        if (request.getMethod() == "GET")
            ft_Get(request, server);
        else if (request.getMethod() == "POST")
         {
            std::cout << "IM hereeee-------------" << std::cout;
               ft_Post(request,server);}
        else if (request.getMethod() == "DELETE")
            ft_delete(request , server);
    }
    else
        std::cout << " method not allowed " << std::endl;
}
