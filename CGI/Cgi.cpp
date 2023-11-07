#include "Cgi.hpp"

std::string    getValue(std::map<std::string, std::string>& headers, const std::string& key)
{
    if (headers.find(key) != headers.end())
        return headers[key];
    return "";
}

Cgi::Cgi(Request &req, const std::string path) : req(req), path(path), status(0)
{
    getEnv();
    execute_cgi(path);
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
	env["req_METHOD"] = req.getMethod();
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

    while(std::getline(temp, name, '/'))
    {
        std::cout << name << std::endl;
    }
    return name;
}
int Cgi::execute_cgi(std::string filename)
{
    int pid;
    int fd[2];

    std::cout << " filename = " <<  filename << std::endl;
    this->name = get_name(filename);
    //std::cout << this->filename << std::endl;
    if (pipe(fd) == -1)
    {
        std::cerr << "failed piping" << std::endl;
        return 500;
    }
    if ((pid = fork()) == -1)
    {
        std::cerr << "failed forking" << std::endl;
        return 500;
    }
    else if (pid == 0)
    {
        char    **env = getEnv();
        int fdin = open(filename.c_str(), O_RDONLY);
        
        char const *cmd[] = {  "python-cgi" , filename.c_str() ,(char *)0  } ;
       
       
       
        close(fd[0]);
		dup2(fdin, 0);
        dup2(fd[1], 1);
        close(fd[1]);
        execve("./cgi-bin/python-cgi",(char *const *) cmd, env);
        std::cerr << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
       
    }
    else
    {
       
        close(fd[1]);
        while (true)
        {
            pid_t result = waitpid(pid, &status, WNOHANG);
            std::cout << result << std::endl;
            if (result == -1)
                return 500;
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
        
        while (rbytes != 0)
        {
            memset(buff, 0, 2048); 
            
            rbytes = read(fd[0], buff, 1023);
            buff[rbytes] = 0;
            response += buff;
        }

        close(fd[0]);
    }
    return 200;
}


//const std::string& Cgi::getCgiResponse() const { return response; }
