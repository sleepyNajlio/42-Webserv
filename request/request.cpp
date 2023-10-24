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

void Request::pLine(std::string line) 
{
    std::stringstream stream(line);
    std::string chk;
    stream >> method >> url >> httpVersion;

    // std::cout << "method: " << method << std::endl;
    // std::cout << "url: " << url << std::endl;
    // std::cout << "httpVersion: " << httpVersion << std::endl;
    // std::cout << "stream: " << stream.str() << std::endl;

    if(stream.fail())
        throw std::invalid_argument("400 Bad Request 1");
    stream >> chk;
    if (!chk.empty())
        throw std::invalid_argument("400 Bad Request 1");
    if (method != "GET" && method != "POST" && method != "DELETE")
        throw std::invalid_argument("501 Not Implemented");
    // request path should only include these "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=%" = 400 bad request
    if (url.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos)
        throw std::invalid_argument("400 Bad Request 2 ");
    else if (url.size() > 2048)
        throw std::invalid_argument("414 Request-URI Too Long");
    else if (httpVersion != "HTTP/1.1")
        throw std::invalid_argument("505 HTTP Version Not Supported");
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

void searchThrow(std::string &out, std::string &line, std::string const &search)
{
    size_t pos = line.find(search);
    if (pos != std::string::npos) {
        out = line.substr(0, pos);
        line = line.substr(pos + search.length(), line.length() - 1);
    } else
        throw std::invalid_argument("400 1");
}

void Request::pHeaders(std::stringstream& stream) 
{

    std::string line;
    std::string key;
    std::string value;
    
    while(std::getline(stream, line) && line != "\r")
    {
        key.clear();
        value.clear();
        searchThrow(key, line, ": ");
        searchThrow(value, line, "\r");
        if (key == "Content-Length") 
        {
            if (value.empty())
                throw std::invalid_argument("411");
            if (value.find_first_not_of("0123456789") != std::string::npos)
                throw std::invalid_argument("400 2");
        } else if (key == "Transfer-Encoding") {
            if (value.empty())
                throw std::invalid_argument("400 3");
            else if (value != "chunked") {
                throw std::invalid_argument("501");
            }
        } else if (key == "Content-Type") {
            if (value.empty())
                throw std::invalid_argument("400 4");
        }
        headers.insert(std::make_pair(key, value));
    }
    if (headers.find("Host") == this->headers.end())
        throw std::invalid_argument("400 5");
    if (method == "POST" && (this->headers.find("Content-Length") == this->headers.end() && this->headers.find("Transfer-Encoding") == this->headers.end()))
            throw std::invalid_argument("411");

    if (method == "GET" && (this->headers.find("Content-Length") !=
    this->headers.end() || this->headers.find("Transfer-Encoding") != this->headers.end()))
        throw std::invalid_argument("400 6");

    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

void Request::parse_headers(std::string buffer) 
{
    std::stringstream stream(buffer);
    std::string line;
    std::getline(stream, line);

    pLine(line);
    pHeaders(stream);
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
            newBuffer = (unsigned char *) memmem(buffer, bytesRead, "\r\n\r\n", 4) + 4; //first part of the body
            // std::cout << buffer[newBuffer - buffer - 1] << std::endl;
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
        std::cout << "header done" << std::endl;
        if (newBuffer)
            frstprtbod = true;
        this->headerDone = true;

    }
    if (this->headerDone == true && this->method == "POST")
    {
        // get the beginning of the body
        unsigned int len = bytesRead;
        if (frstprtbod)
            len = len - (newBuffer - buffer);
        // std::cout << "starting body"<< std::endl;
        this->parse_body(newBuffer, len);
         
    }
    else if (this->headerDone == true && this->method != "POST")
    {
        this->readDone = true;
        std::cout << "done" << std::endl;
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
        // this->isChunked = true;
        std::cout << "chunked" << std::endl;
        if (handleChunked(buffer, bytesRead))
        {
            this->readDone = true;
            close(body);
            std::cout << "done" << std::endl;
        }
    }
    else if (method != "POST")
    {
        this->readDone = true;
        std::cout << "done" << std::endl;
    }
    else
        throw std::runtime_error("Content-Length or Transfer-Encoding not found");
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


bool Request::handleChunked(unsigned char *buffer, int bytesRead)
{

    if (bytesRead < chunkSize && chunkSize > 0)
    {
        std::cout << "here chunkSize: " << chunkSize << std::endl;
        if (write(this->body, buffer, bytesRead) != -1 )
            throw std::runtime_error("Writing to file failed");
        this->bytesSent += bytesRead;
        chunkSize -= bytesRead;
    }
    std::string firstLine(reinterpret_cast<char*>(buffer));
    std::size_t pos = firstLine.find("\r\n");
    bool isHex = true;
    size_t size = bytesRead;
    std::cout << "here2" << std::endl;
    // covert chunk size to hex
    std::string hexNum = firstLine.substr(0, pos);
    if (pos != std::string::npos) {
        std::cout << "hexNum: " << hexNum << std::endl;
        size -= pos + 2;
        for (std::string::iterator it = hexNum.begin(); it != hexNum.end(); ++it) {
            if (!std::isxdigit(*it)) {
                isHex = false;
                break;
            }
        }
        if (isHex)
        // Convert the hexadecimal number to decimal
            chunkSize = std::stoi(hexNum, 0, 16);
    }
    
        if (chunkSize < size)
            size = chunkSize;
        unsigned char *newBuffer = &buffer[pos + 2];
        int rw = write(this->body, newBuffer, size);
        std::cout << "chunkSize: " << chunkSize << std::endl;
        if (rw < 0)
            throw std::runtime_error("Writing to file failed");
        this->bytesSent += size;
        chunkSize -= size;
        // std::cout << "new buffer " << buffer << std::endl;
    if (chunkSize == 0)
        return true;
    return false;
    // newBuffer = someFunction(buffer, len);
    // if (write(newBuffer))
}




