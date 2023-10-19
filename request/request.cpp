#include "request.hpp"


bool pLine(std::istringstream& stream, Request& req) {
    std::string line;
    std::string tmp_one;
    std::string tmp_two;
    std::string tmp_three;

    if (!req.getMethod().empty())
        return true;
    if (!std::getline(stream, line) || line.empty())
        return false;

    if (line.find("HTTP") == std::string::npos)
        return false;

    std::istringstream lineStream(line);
    lineStream >> tmp_one >> tmp_two >> tmp_three;
    req.setMethod(tmp_one);
    req.setUrl(tmp_two);
    req.sethttpVersion(tmp_three);
    return true;
}

bool pHeaders(std::istringstream& stream, Request& req) {
    std::string line;
    req.headers.clear();
    while (std::getline(stream, line) && !line.empty()) {
        std::string::size_type pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            if (req.headers.find(key) != req.headers.end()) {
                req.headers[key] += ", " + value;
            } else {
                req.headers[key] = value;
            }
        } else
            break;
    }
    return true;
}

Request parse_request(std::string buffer, Request req) {

    std::istringstream stream(buffer);
    if (!pLine(stream, req)) {
        std::cerr << "Error 400 bad request: Invalid request line." << std::endl;
        return req;
    }

    if (!pHeaders(stream, req)) {
        std::cerr << "Error 400 bad request: Invalid request headers." <<  std::endl;
        return req;
    }
    // std::string bff = stream.str();
    // if((req.getMethod() != "POST") && bff.find("\n\r\n\r")){
    //     req.setIsDone(true);        
    // }

    // std::map<std::string, std::string>::iterator it = req.headers.find("Content-Length");
    // if (it != req.headers.end()){
    //     unsigned long len;
    //     std::istringstream (it->second) >> len;
    //     req.setContentLenght(len);  
    // }
    // to be continued
    return req;
}
