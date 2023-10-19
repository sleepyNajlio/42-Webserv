#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <map>

class Request{
    private:
            std::string method;
            std::string url;
            std::string httpVersion;

    public:
            void setMethod(const std::string& methode);
            void setUrl(const std::string& url);
            void sethttpVersion( const std::string& httpVersion);
            
            void setBody(const std::string& body);
            void setLocationPath(const std::string& body);
            void setRoot(const std::string& body);
            void setServerName(const std::string& body);

            std::string getMethod() const;
            std::string getUrl() const;
            std::string getBody() const;
            std::string getRoot() const;
            std::string gethttpVersion() const;
            std::string getLocationPath() const;
            std::string getServerName() const;

            std::string body;
            std::string serverName;
            std::string root;
            std::string locPath;
            std::map<std::string, std::string> headers;
};

Request parse_request(std::string buffer, Request req);
#endif