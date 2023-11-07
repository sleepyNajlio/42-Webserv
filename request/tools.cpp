#include    "request.hpp"

Request::Request() {
    this->method = "";
    this->url = "";
    this->httpVersion = "";
    this->headerString = "";
    this->headers.clear();
    this->serverName = "";
    this->locPath = "";
    this->root = "";
    this->bytesSent = 0;
    this->chunkSize = -1;
    this->headerDone = false;
    this->readDone = false;
    this->contentLength = 0;
    this->isChunked = false;
    this->body = -1;
}

Request::Request(const Request& other) {
    // Copy all the member variables from 'other' to 'this'
    *this = other;
}

Request& Request::operator=(const Request& other) {
    if (this == &other) {
        // Self-assignment, no action needed
        return *this;
    }

    // Copy all the member variables from 'other' to 'this'
    method = other.method;
    url = other.url;
    body = other.body;
    root = other.root;
    httpVersion = other.httpVersion;
    locPath = other.locPath;
    serverName = other.serverName;
    chunkSize = other.chunkSize;
    headerDone = other.headerDone;
    readDone = other.readDone;
    headers = other.headers;
    headerString = other.headerString;
    bytesSent = other.bytesSent;
    contentLength = other.contentLength;
    isChunked = other.isChunked;
    randomstr = other.randomstr;
    filename = other.filename;


    return *this;
}

void    Request::setHeaderDone(bool done){
        this->headerDone = done;
}

void    Request::setReadDone(bool done){
        this->readDone = done;
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

void Request::setChunkSize(const int chunkSize){
    this->chunkSize = chunkSize;
}

int Request::getChunkSize() const{
    return chunkSize;
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

void Request::setBody(const int body){
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

std::string Request::getRandomStr() const{
    return randomstr;
}

bool Request::isHeaderDone() const{
    return headerDone;
}

bool Request::isReadDone() const{
    return readDone;
}