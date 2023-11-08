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
        fd_res.open(path, std::ios::in | std::ios::binary | std::ios::ate);
        fd_res.seekg(0, std::ios::end);
        content_length = fd_res.tellg();
        fd_res.seekg(0, std::ios::beg);
        if (fd_res.is_open())
            this->head = "HTTP/1.1 " + std::to_string(code) +  "\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(content_length) + "\r\n\r\n" ;
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
		file.replace(0, locIt->getLocaPath().length(), "");
	else
		file.replace(0, locIt->getLocaPath().length() - 1, "");
    file = locIt->getLocaRoot() + locIt->getLocaPath() + file;
    file = delRepSlash(file);
    std::cout << "file :::"<< file << std::endl;

    if (isDir(file))
    {
        // hna tchek for locIt->getLocaRoot() + locIt->getLocaPath() + locIt->getLocaIndex() if accessable
        if (locIt->getLocaAutoindex())
        {
            listDir(file, request, server);
        }
        else
            errPage(server,404);
    }
    else
    {
        std::ifstream file1(file);

        //-------->CGI HANDLER
        request.ex = get_ex(request.getUrl());
		if(request.ex == "py" || request.ex == "php")
        {
            Cgi cgi(request , file );
            if(cgi.status == 500)
                errPage(server,500);
            else
            {
                //this->head     = cgi.head;
                this->response = cgi.response;
                
                this->head = cgi.head + "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: " + ft_to_string(this->response.size()) + "\r\n\r\n";
            }
        }
        else if (file1.good())
		{
            std::cout << "evvvvv" << std::endl;
            	open_file(server,  file);}
		else if (access(file.c_str(), F_OK))
		{
            std::cout << "evvvvv------------------" << std::endl;
            	errPage(server,404);}
		else
            errPage(server,403);
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
    if (path[path.size() - 1] == '/')
        return 1;
    else
        return 0;
}

std::string get_filename(std::string filename)
{
    std::istringstream temp(filename);
    std::string name;

    while(std::getline(temp, name, '/')){}
    return name;
}

void Response::ft_Post(Request &request, Server_storage &server)
{
    // if (access(request.getUrl().c_str(), F_OK) == -1)
    // {
    //     std::cout << "error 404" << std::endl;
    //     head = ""; // 404
    // }
    if (locIt->loca_upload)
    {
        // check if dir
        if (!slashChecker(request.getUrl()))
        {
            std::string filename = get_filename(request.getUrl());
            std::string name = "./uploads/" + filename;
            if (std::rename(request.randomstr.c_str(), name.c_str()) != 0) 
                std::remove(request.randomstr.c_str());
            head = "HTTP/1.1 201 Created \r\n Content-Type: " + get_content_type(request.getUrl()) + "\r\nContent-Length: 0\r\n\r\n";
        }
        else
        {
            std::remove(request.randomstr.c_str());
            errPage(server,403);
        }
    }
    else if (isDir(request.getUrl()))
    {
        if (slashChecker(request.getUrl()))
        {
            if (locIt->getLocaIndex() != "")
            {
                if (get_ex(request.getUrl()) == "py" || get_ex(request.getUrl()) == "php")
                {
                    // cgi
                }
                else
                {
                    std::cout << "error 403" << std::endl;
                    head = ""; // 403
                }
            }
            else
            {
                std::cout << "error 403" << std::endl;
                head = ""; // 403
            }
        }
        else
        {
            std::cout << "error 301" << std::endl;
            head = ""; // 301
        }
    }
    else
    {
        if (get_ex(request.getUrl()) == "py" || get_ex(request.getUrl()) == "php")
        {
            // cgi
        }
        else
        {
            std::cout << "error 403" << std::endl;
            head = ""; // 403
        }
    }

    // else if (isDir(request.getUrl()))
    // {
    //     if (!slashChecker(request.getUrl()))
    //     {
    //         //  add / to the end of the url
    //         std::cout << "error 301" << std::endl;
    //         head = ""; // 301
    //     }
    //     else if (locIt->getLocaIndex())
    //     {
    //         // cgi
            
            
    //     }
    //     else
    //     {
    //         std::cout << "error 403" << std::endl;
    //         head = ""; // 403
    //     }
    //     slashChecker(request.getUrl());
    // }
    // else if (access(request.getUrl().c_str(), F_OK) == -1)
    // {
    //     std::cout << "error 404" << std::endl;
    //     head = ""; // 404
    // }


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
            // std::cout << "IM hereeee-------------" << std::cout;
    if (allowedMeth(allowedMethods, request.getMethod()))
    {
        // std::cout << request.getMethod() << std::endl;
        method = request.getMethod();
        if (request.getMethod() == "GET")
            ft_Get(request, server);
        else if (request.getMethod() == "POST")
        {
            ft_Post(request,server);
            std::cout << "IM hereeee-------------" << std::endl;
        }
        else if (request.getMethod() == "DELETE")
            ft_delete(request , server);
    }
    else
        std::cout << " method not allowed " << std::endl;
}
