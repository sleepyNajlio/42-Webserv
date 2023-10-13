#include    "request.hpp"

void    request::setMethod(std::string methode){
        this->method = methode;
}
void    request::setUrl(std::string url){
        this->url = url;
}
void    request::sethttpVersion(std::string httpVersion){
        this->httpVersion = httpVersion;
}
void    request::setPort(std::string port){
        this->port = port;
}

std::string request::getMethod(){
    return this->method;
}
std::string request::getUrl(){
    return this->url;
}
std::string request::getPort(){
    return this->port;
}
std::string request::gethttpVersion(){
    return this->httpVersion;
}