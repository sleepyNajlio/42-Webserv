#include "request.hpp"
#include "request.hpp"

static request req;

bool pLine(std::istringstream& stream, request& req) {
    std::string line;
    if (!req.getMethod().empty())
        return true;
    if (!std::getline(stream, line) || line.empty())
        return false;

    if (line.find("HTTP") == std::string::npos)
        return false;

    std::istringstream lineStream(line);
    lineStream >> req.method >> req.url >> req.httpVersion;
    return true;
}

bool pHeaders(std::istringstream& stream, request& req) {
    std::string line;
    req._headers.clear();
    while (std::getline(stream, line) && !line.empty()) {
        std::string::size_type pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            if (req._headers.find(key) != req._headers.end()) {
                req._headers[key] += ", " + value;
            } else {
                req._headers[key] = value;
            }
        } else
            break;
    }
    return true;
}

request parse_request(std::string buffer) {

    std::istringstream stream(buffer);
    if (!pLine(stream, req)) {
        std::cerr << "Error 400 bad request: Invalid request line." << std::endl;
        return req;
    }

    if (!pHeaders(stream, req)) {
        std::cerr << "Error 400 bad request: Invalid request headers." <<  std::endl;
        return req;
    }
    std::string bff = stream.str();
    if((req.getMethod() != "POST") && bff.find("\n\r\n\r")){
        req.setIsDone(true);        
    }
    std::map<std::string, std::string>::iterator it = req._headers.find("Content-Length");
    if (flag_req == 0 && it != req._headers.end()){
        unsigned long len;
        std::istringstream (it->second) >> len;
        req.setContentLenght(len);
    }

    if(req.getMethod() == "POST"){
        storeRequestBody(stream, req, sck, ret_read);
    }    
    return req;
}
