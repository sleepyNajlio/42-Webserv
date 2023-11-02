#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <string>
#include <ctime>

class Request{
    private:
        std::string headerString;
        std::string method;
        std::string url;
        std::string httpVersion;
        bool headerDone;
        bool readDone;
        bool isChunked;
        size_t bytesSent;
        size_t contentLength;
        std::string ContentType;
        std::string filename ;
        // std::string body;
        int body;
        int chunkSize;

    public:
        Request();
        void setHeaderDone(bool done);
        void setReadDone(bool done);
        void setMethod(const std::string& methode);
        void setUrl(const std::string& url);
        void sethttpVersion( const std::string& httpVersion);
        
        void setBody(const int body);
        void setLocationPath(const std::string& body);
        void setRoot(const std::string& body);
        void setServerName(const std::string& body);
        void setChunkSize(const int chunkSize);

        bool isHeaderDone() const;
        bool isReadDone() const;
        std::string getMethod() const;
        std::string getUrl() const;
        int         getBody() const;
        std::string getRoot() const;
        std::string gethttpVersion() const;
        std::string getLocationPath() const;
        std::string getServerName() const;
        int getChunkSize() const;


        // int body;
        std::string serverName;
        std::string root;
        std::string locPath;
        std::map<std::string, std::string> headers;
        void reader(unsigned char *buffer, size_t bytesRead);
        void parse_headers(std::string buffer);
        void parse_body(unsigned char *buffer, size_t bytesRead);
        void pLine(std::string line);
        void pHeaders(std::stringstream& stream);
        bool handleChunked(unsigned char *buffer, size_t bytesRead);
};

#endif