#include "response.hpp"

Response::Response() 
{
    this->status_code = 0;
    this->initStatusCodeMap();
}

void Response::set_status_code(int status_code)
{
    this->status_code = status_code;
}

int Response::get_status_code() const
{
    return this->status_code;
}

void Response::initStatusCodeMap()
{
    errorMessages[400] = "Bad Request";
    errorMessages[414] = "Request-URI Too Large";
    errorMessages[505] = "HTTP Version Not Supported";
    errorMessages[501] = "Not Implemented";
    errorMessages[411] = "Length Required";
    errorMessages[405] = "Method Not Allowed";
    errorMessages[404] = "Not Found";
    errorMessages[403] = "Forbidden";
    errorMessages[301] = "Moved Permanently";
    errorMessages[302] = "Found";
    errorMessages[204] = "No Content";
    errorMessages[201] = "Created";
    errorMessages[200] = "OK";
    errorMessages[500] = "Internal Server Error";
    errorMessages[413] = "Payload Too Large";
    errorMessages[409] = "Conflict";
    errorMessages[408] = "Request Timeout";
    errorMessages[512] = "Bad Gateway";
    errorMessages[503] = "Service Unavailable";
}
