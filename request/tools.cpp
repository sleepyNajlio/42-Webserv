#include    "request.hpp"

Request::Request() {
    this->method = "";
    this->url = "";
    this->httpVersion = "";
    //this->headers = {};
    this->serverName = "";
    this->locPath = "";
    this->root = "";
    this->headerDone = false;
    this->readDone = false;
    this->contentLength = 0;
    this->isChunked = false;
}

void    Request::setHeaderDone(bool done){
       if(done)
        this->headerDone = true;
}

void    Request::setReadDone(bool done){
       if(done)
        this->readDone = true;
}

void    Request::setMethod(const std::string& methode){
        this->method = methode;
}
void    Request::setUrl(const std::string& url){
        this->url = url;
}
void    Request::sethttpVersion(const std::string& httpVersion){
        this->httpVersion = httpVersion;
}


std::string Request::getMethod() const {
    return this->method;
}
std::string Request::getUrl() const {
    return this->url;
}
std::string Request::gethttpVersion() const {
    return this->httpVersion;
}

// ==============================================================

void Request::setBody(const int& body){
    this->body = body;
}

void Request::setServerName(const std::string& serverName){
    this->serverName = serverName;
}

void Request::setLocationPath(const std::string& path){
    this->locPath = path;
}

void Request::setRoot(const std::string& path){
    this->root = path;
}

int Request::getBody() const{
    return body;
}

std::string Request::getServerName() const{
    return serverName;
}

std::string Request::getLocationPath() const{
    return locPath;
}

std::string Request::getRoot() const{
    return root;
}

bool Request::isHeaderDone() const{
    return headerDone;
}

bool Request::isReadDone() const{
    return readDone;
}