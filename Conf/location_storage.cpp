#include "incs/location_storage.hpp"

Location_storage::Location_storage()
{
	init_location();
}

Location_storage::~Location_storage()
{
}

void Location_storage::init_location()
{
    loca_path = "";
	loca_root = "";
	loca_autoindex = false;
	loca_index = "";
	loca_redirect = "";
	loca_alias = "";
	loca_client_max_body_size = 2048; // change it //
	loca_allowed_methods.push_back(0);
	loca_allowed_methods.push_back(0);
	loca_allowed_methods.push_back(0);
}

// setters //
void Location_storage::setLocaPath(std::string loca_path)
{
    this->loca_path = loca_path;
}

void Location_storage::setLocaRoot(std::string loca_root)
{
	 if (Conf_reader::identifyPath(loca_root) != 2)
		 throw InvalidRootDir();
	this->loca_root = loca_root;
}

void Location_storage::setLocaAllowedMethods(storage allowed_methods)
{
	for (size_t i = 0; i < allowed_methods.size(); i++)
	{
        switch(allowed_methods[i][0])
        {
            case 'G':
                this->loca_allowed_methods[0] = 1;
                break;
            case 'P':
                this->loca_allowed_methods[1] = 1;
                break;
            case 'D':
                this->loca_allowed_methods[2] = 1;
                break;
            default:
                throw InvalidMethodException();
                // break;
        }
	}
}

void Location_storage::setLocaAutoindex(std::string loca_autoindex)
{
    if (loca_autoindex != "on" && loca_autoindex != "off")
        throw InvalidAutoindex();
    this->loca_autoindex = (loca_autoindex == "on") ? true : false; 
}

void Location_storage::setLocaIndex(std::string loca_index)
{
	this->loca_index = loca_index;
}

void Location_storage::setLocaAlias(std::string loca_alias)
{
	this->loca_alias = loca_alias;
}

void Location_storage::setLocaCgipath(storage loca_cgi_path)
{
	this->loca_cgi_path = loca_cgi_path;
}

void Location_storage::setLocaRedirect(std::string loca_redirect)
{
	this->loca_redirect = loca_redirect;
}

bool Location_storage::check_maxsize(std::string maxsize)
{
    for (size_t i = 0; i < maxsize.size(); i++)
    {
        if (maxsize[i] > '9' || maxsize[i] < '0')
            throw InvalidMaxSize();
    }
    return (true);
}

void Location_storage::setLoca_client_max_body_size(std::string loca_client_max_body_size)
{
    unsigned long body_size = 0;

	for (size_t i = 0; i < loca_client_max_body_size.length(); i++)
	{
		if (loca_client_max_body_size[i] < '0' || loca_client_max_body_size[i] > '9')
			throw 	InvalidMaxSize();
	}
	if (!convertStrToInt(loca_client_max_body_size))
		throw InvalidMaxSize();
	body_size = convertStrToInt(loca_client_max_body_size);
	this->loca_client_max_body_size = body_size;
}

void Location_storage::setLoca_client_max_body_size(long unsigned loca_client_max_body_size)
{
	this->loca_client_max_body_size = loca_client_max_body_size;
}

// getters //
const std::string &Location_storage::getLocaPath() const
{
	return (this->loca_path);
}

const std::string &Location_storage::getLocaRoot() const
{
	return (this->loca_root);
}

const storage_int &Location_storage::getLocaAllowedMethods() const
{
	return (this->loca_allowed_methods);
}

const bool &Location_storage::getLocaAutoindex() const
{
	return (this->loca_autoindex);
}

const std::string &Location_storage::getLocaIndex() const
{
	return (this->loca_index);
}

const std::string &Location_storage::getLocaAlias() const
{
	return (this->loca_alias);
}

const storage &Location_storage::getLocaCgipath() const
{
	return (this->loca_cgi_path);
}

const std::string &Location_storage::getLocaRedirect() const
{
	return (this->loca_redirect);
}

const unsigned long &Location_storage::getLoca_client_max_body_size() const
{
	return (this->loca_client_max_body_size);
}