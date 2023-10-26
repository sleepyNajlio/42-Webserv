#include "cgi.hpp"

std::string    getValue(std::map<std::string, std::string>& headers, const std::string& key)
{
    if (headers.find(key) != headers.end())
        return headers[key];
    return "";
}

Cgi::Cgi(Request request, const std::string path) : request(request), path(path), status(0)
{
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
    std::map<std::string, std::string>	headers = request.headers;

    env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["REQUEST_METHOD"] = request.getMethod();
	env["PATH_INFO"] = path;
	env["PATH_TRANSLATED"] = path;
	env["SCRIPT_NAME"] = path;
	//env["QUERY_STRING"] = request.getQuery();
	env["REMOTE_HOST"] = getValue(headers, "User-Agent");
	env["REDIRECT_STATUS"] = "200";
    env["HTTP_ACCEPT_LANGUAGE"] = getValue(headers, "Accept-Language");
    env["HTTP_ACCEPT_CHARSET"] = getValue(headers, "Accept-Charset");
    env["CONTENT_LANGUAGE"] = getValue(headers, "Content-Language");
    env["HTTP_COOKIE"] = getValue(headers, "Cookie");
	if (request.getMethod() == "POST")
	{
    	env["CONTENT_LENGTH"] = getValue(headers, "Content-Length");
		env["CONTENT_TYPE"] = getValue(headers, "Content-Type");
	}
	return env_to_char(env);

}

int Cgi::execute_cgi(std::string filename)
{
    int pid;
    int fd[2];

    if (pipe(fd) == -1)
    {
        std::cerr << "pipe failed!" << std::endl;
        return 500;
    }
    if ((pid = fork()) == -1)
    {
        std::cerr << "Fork failed!" << std::endl;
        return 500;
    }
    else if (pid == 0)
    {
        char    **env = getEnv();
        char* const nul[] = { const_cast<char*>(env[0]), NULL };
	    int fdin = open(filename.c_str(), O_RDONLY);
        close(fd[0]);
		dup2(fdin, 0);
        dup2(fd[1], 1);
        close(fd[1]);
        execve(path.c_str(), nul, env);
        std::cerr << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        close(fd[1]);
      time_t startTime = time(NULL);
        while (true)
        {
            pid_t result = waitpid(pid, &status, WNOHANG);
            if (result == -1)
                return 500;
            else if (result == 0)
            {
                time_t  currentTime = time(NULL);
                if (currentTime - startTime > 2)
                {
                    kill(pid, SIGKILL);
                    return 502;
                }
            }
            else
            {
                if (WEXITSTATUS(status) != EXIT_SUCCESS)
                    return 500;
                break;
            }
            usleep(10000);
        }
          char buff[1024];
        int rbytes = 1;
        while (rbytes != 0)
        {
            memset(buff, 0, 1024);
            rbytes = read(fd[0], buff, 1023);
            buff[rbytes] = 0;
            response += buff;
        }
        std::cout << "-----------------cgi-----------------"<< std::endl;
        std::cout << response << std::endl;

        close(fd[0]);
    }
    return 200;
}


const std::string& Cgi::getCgiResponse() const { return response; }
