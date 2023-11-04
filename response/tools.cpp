#include "response.hpp"

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

std::vector<Location_storage>::const_iterator locationMatch(Server_storage &serv, std::string path)
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

std::string delRepSlash(std::string file)
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