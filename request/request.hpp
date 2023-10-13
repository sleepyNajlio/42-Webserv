#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>

class Request{
    private:
            std::string method;
            std::string url;
            std::string httpVersion;
            std::string port;
    public:
            void setMethod(std::string methode);
            void setUrl(std::string url);
            void sethttpVersion(std::string httpVersion);
            void setPort(std::string port);

            std::string getMethod();
            std::string getUrl();
            std::string getPort();
            std::string gethttpVersion();

            std::string _body;
            std::string _serverName;
            std::string _root;
            std::string _locPath;
            std::map<std::string, std::string> headers;
}

void parse_request(std::string buffer);
#endif