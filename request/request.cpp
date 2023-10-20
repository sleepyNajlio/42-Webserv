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
        this->parse_headers(this->headerString, this->headerString.size());
        try 
        {
            // this->handle_headers(this->headerString, server);
        }
        catch (std::exception& e) 
        {
            std::cerr << e.what() << std::endl;            
        }
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
        this->parse_body(newBuffer, frstprtbod, len);
         
    }
    else if (this->headerDone == true && this->method != "POST")
        this->readDone = true;
}

void Request::parse_headers(std::string buffer) 
{
    // we should check if the header is complete or not before doing this otherwise we should not cast the body to string
    std::istringstream stream(buffer);
    if (!pLine(stream, *this)) {
        std::cerr << "Error 400 bad request: Invalid request line." << std::endl;
        return;
    }

    if (!pHeaders(stream, *this)) {
        std::cerr << "Error 400 bad request: Invalid request headers." <<  std::endl;
        return;
    }
 
}

void Request::parse_body(unsigned char *buffer, int bytesRead) 
{
    if (this->headers.find("Content-Length") != this->headers.end())
    {
        if (buffer)
        {
            std::cout << "buffer size: " << sizeof(buffer) << std::endl;
            this->contentLength -= sizeof(buffer);
        }
    }
    std::cout << "body " << std::endl;
    if (write(this->body, buffer, bytesRead) != -1 )
        this->bytesSent += bytesRead;
    else
        throw std::runtime_error("Writing to file failed");
    
    if (this->isChunked == true && memmem(newBuffer, bytesRead, "0\r\n\r\n", 5) != NULL)
    {
        this->read_done = true;
        close(reqBodyFd);
        std::cout << "done" << std::endl;
        //handle body

    }
    else if (this->contentLength == this->bytesSent)
    {
        this->read_done = true;
        close(reqBodyFd);
        std::cout << "done" << std::endl;
        //handle body
    }   
    std::cout << "body: " << this->body << std::endl;
    std::cout << "body size: " << this->body.size() << std::endl;
    std::cout << "content length: " << this->contentLength << std::endl;

}
