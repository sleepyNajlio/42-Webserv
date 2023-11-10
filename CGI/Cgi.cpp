#include "Cgi.hpp"

std::string    getValue(std::map<std::string, std::string>& headers, const std::string& key)
{
    if (headers.find(key) != headers.end())
        return headers[key];
    return "";
}

Cgi::Cgi(Request &req, const std::string path, std::string fp) : req(req), path(path), status(0)
{
    getEnv();
    status = execute_cgi(path, req.ex, fp);
}

char    **Cgi::env_to_char (std::map<std::string, std::string>& env)
{  
    std::map<std::string, std::string>::iterator it;
	char **t_env = new char*[env.size() + 1];
	int i = 0;
	for(it = env.begin(); it != env.end(); it++)
    {
		std::string keyValue = it->first + "=" + it->second;
		t_env[i] = new char[keyValue.size() + 1];
		strcpy(const_cast<char*>(t_env[i]), keyValue.c_str());
		i++;
	}
	t_env[i] = NULL;
    return t_env;
}

char    **Cgi::getEnv()
{
    std::map<std::string, std::string>	headers = req.headers;

    env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["REQUEST_METHOD"] = req.getMethod();
	env["PATH_INFO"] = path;
	env["PATH_TRANSLATED"] = path;
	env["SCRIPT_NAME"] = path;
	env["QUERY_STRING"] = req.query;
	env["REMOTE_HOST"] = getValue(headers, "User-Agent");
	env["REDIRECT_STATUS"] = "200";
    env["HTTP_ACCEPT_LANGUAGE"] = getValue(headers, "Accept-Language");
    env["HTTP_ACCEPT_CHARSET"] = getValue(headers, "Accept-Charset");
    env["CONTENT_LANGUAGE"] = getValue(headers, "Content-Language");
    env["HTTP_COOKIE"] = getValue(headers, "Cookie");
	if (req.getMethod() == "POST")
	{
    	env["CONTENT_LENGTH"] = getValue(headers, "Content-Length");
		env["CONTENT_TYPE"] = getValue(headers, "Content-Type");
	}
	return env_to_char(env);

}

std::string get_name(std::string filename)
{
    std::istringstream temp(filename);
    std::string name;

    while(std::getline(temp, name, '/')){}
    return name;
}

int Cgi::execute_cgi(std::string filename , std::string ex, std::string fp)
{
    int pid;
    int fd[2];

	// (void)fp;
    this->name = get_name(filename);
    if (pipe(fd) == -1)
    {
        std::cerr << "failed piping" << std::endl;
        return 500;
    }
	std::cout << "waa3" << filename.c_str() << std::endl;
    if ((pid = fork()) == -1)
    {
        std::cerr << "failed forking" << std::endl;
        return 500;
    }
    else if (pid == 0)
    {
        char    **env = getEnv();
        int fdin = open(fp.c_str(), O_RDONLY); 
        // if(fdin < 0)
        //     exit(404);
        std::string cgi_path;
        char const *cmd[3];
        if (ex == "py")
        {
            cmd[0] = "python-cgi";
            cmd[1] = filename.c_str();
            cmd[2] = (char *)0;
            cgi_path = "/usr/bin/python3";
        }
        else
        {
            cmd[0] = "php-cgi";
            cmd[1] = filename.c_str();
            cmd[2] = (char *)0;
            cgi_path = "./cgi-bin/php-cgi";
        }
        close(fd[0]);
		if (fdin > 1)
			dup2(fdin, 0);
        dup2(fd[1], 1);
        close(fd[1]);
		std::cerr << cmd[0] << " " << cmd[1] << " " << cgi_path << std::endl;
        execve(cgi_path.c_str(),(char *const *) cmd, env);
        std::cerr << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        close(fd[1]);
        while (true)
        {
            pid_t result = waitpid(pid, &status, WNOHANG);
            if (result == -1)
                return 500;
			else if (status == 404)
				return 404;
            else
            {
                if (WEXITSTATUS(status) != EXIT_SUCCESS)
                    return 500;
                break;
            }
            usleep(10000);
        }
        char buff[2048];
        int rbytes = 1;
        
        if (rbytes != 0)
        {
			while (memset(buff, 0, 2048) && read(fd[0], buff, 2048) > 0)
				this->response += buff;
            if(ex == "php")
            {
                std::string temp = buff;
                if(temp.find("\r\n\r\n"))
                {
                    this->head += "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: " + std::to_string(temp.substr(temp.find("\r\n\r\n")+4).size()) + "\r\n";
                    this->head += temp.substr(0,temp.find("\r\n\r\n"));
                }
                if(temp.find("\r\n\r\n"))
                    this->response += temp.substr(temp.find("\r\n\r\n")+4);
                else
                 this->response += buff;
            }
			std::cout << "resp :" << response << std::endl;
        }
        close(fd[0]);
    }
    return 200;
}