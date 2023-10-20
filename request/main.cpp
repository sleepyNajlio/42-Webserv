#include "request.hpp"

int main() {
    std::string httpRequest = "GET /index.html HTTP/1.1\r\n"
                            "Host: example.com\r\n"
                            "User-Agent: Mozilla/5.0\r\n"
                            "\r\n"
                            "This is the request body.";

    Request req;
    req = parse_request(httpRequest, req);

    std::cout << "Method: " << req.getMethod() << std::endl;
    std::cout << "URL: " << req.getUrl() << std::endl;
    std::cout << "HTTP Version: " << req.gethttpVersion() << std::endl;

    std::cout << "Headers:" << std::endl;
    std::map<std::string, std::string>::const_iterator it;
    for (it = req.headers.begin(); it != req.headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    std::cout << "Request Body: " << req.getBody() << std::endl;

    return 0;
}
