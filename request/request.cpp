#include "request.hpp"

std::string generateRandomString()
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int length = 5 + std::rand() % (30 - 5 + 1);

    // Seed the random number generator with the current time
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    char *randomString = new char[length + 1]; // +1 for the null terminator

    for (int i = 0; i < length; ++i)
    {
        int randomIndex = std::rand() % (sizeof(charset) - 1);
        randomString[i] = charset[randomIndex];
    }

    randomString[length] = '\0'; // Null-terminate the string

    return randomString;
}

void Request::pLine(std::string line)
{
    std::string path;
    std::string chk;

    std::stringstream stream(line);
    stream >> method >> url >> httpVersion;

    std::istringstream temp(this->getUrl());
    std::getline(temp, path, '?');
    temp >> this->query;
    setUrl(path);

    if (stream.fail())
        throw std::invalid_argument("400 Bad Request 1");
    stream >> chk;
    if (!chk.empty())
        throw std::invalid_argument("400");
    if (method != "GET" && method != "POST" && method != "DELETE")
        throw std::invalid_argument("501 Not Implemented");

    if (url.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos)
        throw std::invalid_argument("400 Bad Request 2 ");
    else if (url.size() > 2048)
        throw std::invalid_argument("414 Request-URI Too Long");
    else if (httpVersion != "HTTP/1.1")
        throw std::invalid_argument("505 HTTP Version Not Supported");
    // open THE POSTED FILE and write the body in it
    if (this->method == "POST")
    {
        this->randomstr = "./cache/" + generateRandomString();
        this->body = open(this->randomstr.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (this->body == -1)
            throw std::invalid_argument("Unable to open file ");
    }
}

void searchThrow(std::string &out, std::string &line, std::string const &search)
{
    size_t pos = line.find(search);
    if (pos != std::string::npos)
    {
        out = line.substr(0, pos);
        line = line.substr(pos + search.length(), line.length() - 1);
    }
    else
        throw std::invalid_argument("400");
}

void Request::pHeaders(std::stringstream &stream)
{
    std::string line;
    std::string key;
    std::string value;
    bool flag = true;
    while (std::getline(stream, line) && line != "\r")
    {
        key.clear();
        value.clear();
        searchThrow(key, line, ": ");
        searchThrow(value, line, "\r");
        if (flag)
        {
            this->filename = value;
            flag = false;
        }
        if (key == "Content-Length")
        {
            if (value.empty())
                throw std::invalid_argument("411");
            if (value.find_first_not_of("0123456789") != std::string::npos)
                throw std::invalid_argument("400");
        }
        else if (key == "Transfer-Encoding")
        {
            if (value.empty())
                throw std::invalid_argument("400");
            else if (value != "chunked")
            {
                throw std::invalid_argument("501");
            }
        }
        else if (key == "Content-Type")
        {
            if (value.empty())
                throw std::invalid_argument("400");
        }
        headers.insert(std::make_pair(key, value));
    }
    if (headers.find("Host") == this->headers.end())
        throw std::invalid_argument("400");
    if (method == "POST" && (this->headers.find("Content-Length") == this->headers.end() && this->headers.find("Transfer-Encoding") == this->headers.end()))
        throw std::invalid_argument("411");
}

void Request::parse_headers(std::string buffer)
{
    std::stringstream stream(buffer);
    std::string line;
    std::getline(stream, line);

    pLine(line);
    pHeaders(stream);
}

void Request::reader(unsigned char *buffer, size_t bytesRead)
{
    unsigned char *newBuffer;
    newBuffer = buffer;
    bool frstprtbod = false;
    if (this->headerDone == false)
    {
        // init request header
        if (memmem(buffer, bytesRead, "\r\n\r\n", 4) != NULL)
        {
            newBuffer = (unsigned char *)memmem(buffer, bytesRead, "\r\n\r\n", 4) + 4; // first part of the body
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
        if (newBuffer)
            frstprtbod = true;
        this->headerDone = true;
    }
    if (this->headerDone == true && this->method == "POST" && newBuffer)
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

void Request::parse_body(unsigned char *buffer, size_t bytesRead)
{
    if (this->method == "POST" && this->headers.find("Content-Length") != this->headers.end())
    {
        this->contentLength = std::stoi(this->headers["Content-Length"]);
        if (write(this->body, buffer, bytesRead) != -1)
            this->bytesSent += bytesRead;
        else
            throw std::runtime_error("500 Writing to file failed");
        if (bytesSent == contentLength)
        {
            this->readDone = true;
            close(body);
        }
        else if (bytesSent > contentLength)
            throw std::runtime_error("409 Conflict");
    }
    else if (this->method == "POST" && this->headers.find("Transfer-Encoding") != this->headers.end() && this->headers["Transfer-Encoding"] == "chunked")
    {
        if (handleChunked(buffer, bytesRead))
        {
            this->readDone = true;
            close(body);
        }
    }
    else if (method != "POST")
        this->readDone = true;
    else
        throw std::runtime_error("411 Content-Length or Transfer-Encoding not found");
}

bool verif_chunk(unsigned char *buffer)
{
    std::string firstLine(reinterpret_cast<char *>(buffer));
    std::size_t pos = firstLine.find("\r\n");
    std::string hexNum = firstLine.substr(0, pos);

    if (pos != std::string::npos)
    {
        for (std::string::iterator it = hexNum.begin(); it != hexNum.end(); ++it)
        {
            if (!std::isxdigit(*it))
                return false;
        }
        return true;
    }
    return false;
}

void test_chunk(unsigned char *buffer, size_t bytesRead, bool &readDone, int body)
{
    if (memmem(buffer, bytesRead, "0\r\n\r\n", 5) == NULL)
    {
        if (memmem(buffer, bytesRead, "\r\n", 2) != NULL && verif_chunk((unsigned char *)memmem(buffer, bytesRead, "\r\n", 2) + 2))
        {
            int first = (unsigned char *)memmem(buffer, bytesRead, "\r\n", 2) - buffer;
            int second = bytesRead - first;
            write(body, buffer, first);
            buffer = (unsigned char *)memmem(buffer, bytesRead, "\r\n", 2) + 2;
            second = second - ((unsigned char *)memmem(buffer, second, "\r\n", 2) + 2 - buffer);
            // now buffer starts with the chunk size
            buffer = (unsigned char *)memmem(buffer, second, "\r\n", 2) + 2;
            // now buffer starts with the chunk
            // int second = &buffer[bytesRead - 1] - buffer;
            write(body, buffer, second - 2);
        }
        else
        {
            if (verif_chunk(buffer))
            {
                bytesRead = bytesRead - ((unsigned char *)memmem(buffer, bytesRead, "\r\n", 2) + 2 - buffer);
                buffer = (unsigned char *)memmem(buffer, bytesRead, "\r\n", 2) + 2;
            }
            if (write(body, buffer, bytesRead) == -1)
                throw std::runtime_error("500 Writing to file failed");
        }
    }
    else
    {
        bytesRead = bytesRead - sizeof("\r\n0\r\n\r\n") + 1;
        if (write(body, buffer, bytesRead) == -1)
            throw std::runtime_error("500 Writing to file failed 4");
        readDone = true;
    }
}

bool Request::handleChunked(unsigned char *buffer, size_t bytesRead)
{
    test_chunk(buffer, bytesRead, readDone, body);
    return false;
}
