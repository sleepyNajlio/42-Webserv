#include "request.hpp"

char* generateRandomString() {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int length = 5 + std::rand() % (30 - 5 + 1);

    // Seed the random number generator with the current time
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    char* randomString = new char[length + 1]; // +1 for the null terminator

    for (int i = 0; i < length; ++i) {
        int randomIndex = std::rand() % (sizeof(charset) - 1);
        randomString[i] = charset[randomIndex];
    }

    randomString[length] = '\0'; // Null-terminate the string

    return randomString;
}

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
    if (lineStream.fail() || !lineStream.eof())
        return false;

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
    // print this
    return true;
}

void Request::reader(unsigned char *buffer, ssize_t bytesRead)
{
    unsigned char *newBuffer;
    newBuffer = buffer;
    bool frstprtbod = false;
    // std::cout << "reader " << std::endl;
    if (this->headerDone == false)
    {
        // init request header
        if (memmem(buffer, bytesRead, "\r\n\r\n", 4) != NULL)
        {
            newBuffer = (unsigned char *) memmem(buffer, bytesRead, "\r\n\r\n", 4) + 4;
            std::cout << buffer[newBuffer - buffer - 1] << std::endl;
            buffer[newBuffer - buffer - 1] = '\0';
            this->headerString += (char *)buffer;

        }
        else
        {
            this->headerString += (char *)buffer;
            return;
        }
        this->headerString += "\n";
        this->parse_headers(this->headerString);
        // try 
        // {
        //     // this->handle_headers(this->headerString, server);
        // }
        // catch (std::exception& e) 
        // {
        //     std::cerr << e.what() << std::endl;            
        // }
        frstprtbod = true;
        this->headerDone = true;

        // open a random file and write the body in it
        if (this->method == "POST")
        {
            std::cout << "post" << std::endl;
            char *tempFile = generateRandomString();
            this->body= open(tempFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (this->body == -1)
                throw std::invalid_argument("Unable to open file ");
        }
    }
    if (this->headerDone == true && this->method == "POST")
    {
        // get the beginning of the body
        unsigned int len = bytesRead;
        if (frstprtbod)
            len = len - (newBuffer - buffer);
        this->parse_body(newBuffer, len);
         
    }
    else if (this->headerDone == true && this->method != "POST")
        this->readDone = true;
}

void Request::parse_headers(std::string buffer) 
{
    size_t i = buffer.find("\r\n");
    std::string line = buffer.substr(0, i);
    std::stringstream s(line);
    s >> method >> url >> httpVersion;

    if(s.fail() || !s.eof())
        throw std::invalid_argument("400 Bad Request");
    if (method != "GET" && method != "POST" && method != "DELETE")
        throw std::invalid_argument("501 Not Implemented");
    // request path should only include these "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=%" = 400 bad request
    if (url.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos)
        throw std::invalid_argument("400 Bad Request");
    else if (url.size() > 2048)
        throw std::invalid_argument("414 Request-URI Too Long");
    else if (httpVersion != "HTTP/1.1")
        throw std::invalid_argument("505 HTTP Version Not Supported");
    std::istringstream stream(buffer);
    // if (!pLine(stream, *this)) {
    //     std::cerr << "Error 400 bad request: Invalid request line." << std::endl;
    //     return;
    // }
    if (!pHeaders(stream, *this)) {
        std::cerr << "Error 400 bad request: Invalid request headers." <<  std::endl;
        return;
    }
 
}

void Request::parse_body(unsigned char *buffer, int bytesRead) 
{
    if (this->headers.find("Content-Length") != this->headers.end())
    {
        this->contentLength = std::stoi(this->headers["Content-Length"]);
        if (write(this->body, buffer, bytesRead) != -1 )
            this->bytesSent += bytesRead;
        else
            throw std::runtime_error("Writing to file failed");
        if (bytesSent == contentLength)
        {
            this->readDone = true;
            close(body);
            std::cout << "done" << std::endl;
        }
        else if (bytesSent > contentLength)
            throw std::runtime_error("Content-Length exceeded");
    }
    else if (this->headers.find("Transfer-Encoding") != this->headers.end() && this->headers["Transfer-Encoding"] == "chunked")
    {
        this->isChunked = true;
        // handleChunked(buffer, bytesRead);
    }
    else
    {
        this->readDone = true;
        close(body);
        std::cout << "done" << std::endl;
    }
    // else
    // {
    //     // handle body
    //     if (write(this->body, buffer, bytesRead) != -1 )
    //         this->bytesSent += bytesRead;
    //     else
    //         throw std::runtime_error("Writing to file failed");
    //     if (bytesSent == contentLength)
    //     {
    //         this->readDone = true;
    //         close(body);
    //         std::cout << "done" << std::endl;
    //     }
    // }
    
    // else if (this->contentLength == this->bytesSent)
    // {
    //     this->readDone = true;
    //     close(body);
    //     std::cout << "done" << std::endl;
    //     //handle body
    // }   
    // std::cout << "body: " << this->body << std::endl;
    // std::cout << "body size: " << this->body.size() << std::endl;
    // std::cout << "content length: " << this->contentLength << std::endl;

}

void handleChunked(unsigned char buffer, int len)
{
    unsigned char *newBuffer;
    newBuffer = someFunction(buffer, len);
    if (write(newBuffer))

}
