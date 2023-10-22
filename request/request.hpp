#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <fcntl.h>
#include <unistd.h>

class Request{
    private:
        std::string headerString;
        std::string method;
        std::string url;
        std::string httpVersion;
        int contentLength;
        int bytesSent;
        bool headerDone;
        bool readDone;
        bool isChunked;
        // std::string body;
        int body;

    public:
        Request();
        void setHeaderDone(bool done);
        void setReadDone(bool done);
        void setMethod(const std::string& methode);
        void setUrl(const std::string& url);
        void sethttpVersion( const std::string& httpVersion);
        
        void setBody(const int& body);
        void setLocationPath(const std::string& body);
        void setRoot(const std::string& body);
        void setServerName(const std::string& body);

        bool isHeaderDone() const;
        bool isReadDone() const;
        std::string getMethod() const;
        std::string getUrl() const;
        int getBody() const;
        std::string getRoot() const;
        std::string gethttpVersion() const;
        std::string getLocationPath() const;
        std::string getServerName() const;

        // int body;
        std::string serverName;
        std::string root;
        std::string locPath;
        std::map<std::string, std::string> headers;
        void reader(unsigned char *buffer, ssize_t bytesRead);
        void parse_headers(std::string buffer,ssize_t bytesRead);
        void parse_body(unsigned char *buffer, int bytesRead);
        bool pLine(std::istringstream& stream, Request& req);
        char* generateRandomString() ;
};

#endif