#include "inc/Multiplexing.hpp"


Response::Response()
{
    headerSent = false;
    responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
    resBodyFd = -1;
    resBodySize = 0;

}

Response::~Response()
{
}


Multiplexing::Multiplexing()
{
    FD_ZERO(&io.readSockets);
    FD_ZERO(&io.writeSockets);
    FD_ZERO(&io.tmpReadSockets);
    FD_ZERO(&io.tmpWriteSockets);
    maxFd = 0;
    reqBodyFd = -1;

    // io.readSocketsSize = 0;
    // io.writeSocketsSize = 0;
}

Multiplexing::~Multiplexing()
{
}

ioSets Multiplexing::getIoSets() const
{
    return io;
}

std::vector<std::pair < Client, Server_storage > > Multiplexing::getClients() const
{
    return clients;
}

int Multiplexing::getBodyFd() const
{
    return (this->reqBodyFd);
}

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

void Multiplexing::reader(unsigned char *buffer, Client &client, Server_storage server, ssize_t bytesRead)
{
    unsigned char *newBuffer;
    newBuffer = buffer;
    bool frstprtbod = false;
    // std::cout << "reader " << std::endl;
    if (client.req.sentHeader == false)
    {
        // init request header
        if (memmem(buffer, bytesRead, "\r\n\r\n", 4) != NULL)
        {
            newBuffer = (unsigned char *) memmem(buffer, bytesRead, "\r\n\r\n", 4) + 4;
            std::cout << buffer[newBuffer - buffer - 1] << std::endl;
            buffer[newBuffer - buffer - 1] = '\0';
            client.req.headerString += (char *)buffer;

        }
        else
        {
            client.req.headerString += (char *)buffer;
            return;
        }
        client.req.headerString += "\n";
        try 
        {
            client.req.handle_headers(client.req.headerString);
        }
        catch (std::exception& e) 
        {
            std::cerr << e.what() << std::endl;            
        }
        frstprtbod = true;
        client.req.sentHeader = true;

        // open a random file and write the body in it
        std::cout << "open" << std::endl;
        char *tempFile = generateRandomString();
        this->reqBodyFd = open(tempFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (this->reqBodyFd == -1)
            throw std::invalid_argument("Unable to open file ");
        client.req.body = tempFile;

    }
    if (client.req.sentHeader == true && client.req.method == "POST")
    {
        // get the beginning of the body
        unsigned int len = bytesRead;
        if (frstprtbod)
            len = len - (newBuffer - buffer);

        // std::cout << "body " << std::endl;
        if (write(reqBodyFd, newBuffer, len) != -1 )
            client.req.bytesSent += len;
        else
            throw std::runtime_error("Writing to file failed");
        
        if (client.req.isChunked == true && memmem(newBuffer, len, "0\r\n\r\n", 5) != NULL)
        {
            client.req.read_done = true;
            close(reqBodyFd);
            std::cout << "done" << std::endl;
            //handle body

        }
        else if (client.req.contentLength == client.req.bytesSent)
        {
            client.req.read_done = true;
            close(reqBodyFd);
            std::cout << "done" << std::endl;
            //handle body
            
        }    
    }
    else if (client.req.sentHeader == true && client.req.method != "POST")
        client.req.read_done = true;
}



void Multiplexing::setupServer(std::vector <std::pair <Socket , Server_storage > > _server)
{
    for ( std::vector<std::pair <Socket , Server_storage > >::iterator it = _server.begin(); it != _server.end(); ++it)
    {
        FD_SET(it->second.getFd(), &io.readSockets);
        maxFd = std::max( maxFd, it->second.getFd());
        // io.readSocketsSize++;
        // std::cout << "maxFd " << maxFd << std::endl;
    }



    // const char* responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: video/mp4\r\nContent-Length: ";
    // const char* responseEnd = "\r\n\r\n";

    while (true)
    {
        io.tmpReadSockets = io.readSockets;
        io.tmpWriteSockets = io.writeSockets;
        

        if (select(maxFd + 1, &io.tmpReadSockets, &io.tmpWriteSockets, NULL, NULL) == -1)
        {
            perror("Waiting for incoming data failed");
            break;
        }
        std::cout << "select" << std::endl;
        for (std::vector <std::pair <Socket , Server_storage > >::iterator it = _server.begin(); it != _server.end(); ++it)
        {
            if (FD_ISSET(it->second.getFd(), &io.tmpReadSockets))
            {
                handleNewConnection(it->first, it->second);
            }
        }
        for (int i = 0; i < (int)clients.size(); i++)
        {
            // check for read event
            if (FD_ISSET(clients[i].first.get_fd(), &io.tmpReadSockets))
            {
                
                unsigned char buffer[2048];
                bzero(buffer, 2048);
                // std::cout << "buffer size " << sizeof(buffer) << std::endl;
                ssize_t bytesRead = recv(clients[i].first.get_fd(), buffer, sizeof(buffer), 0);
                if (bytesRead == -1) {
                    perror("Receiving data failed");
                    break;
                }
                else if (bytesRead == 0) {
                    std::cout << "Client closed connection" << std::endl;
                    close(clients[i].first.get_fd());
                    FD_CLR(clients[i].first.get_fd(), &io.readSockets);
                    FD_CLR(clients[i].first.get_fd(), &io.writeSockets);
                    clients.erase(clients.begin() + i);
                    break;
                }
                else {
                    buffer[bytesRead] = '\0';
                    try 
                    {
                        reader(buffer, clients[i].first, clients[i].second, bytesRead);

                    }
                    catch (std::runtime_error& e) {
                        std::cerr << e.what() << std::endl;
                        break;
                    }
                    if (clients[i].first.req.read_done)
                    {
                        FD_CLR(clients[i].first.get_fd(), &io.readSockets);
                        FD_SET(clients[i].first.get_fd(), &io.writeSockets);
                    }
                }
            }
            // check for write event
            if (FD_ISSET(clients[i].first.get_fd(), &io.tmpWriteSockets))
            {
                std::cout << "write" << std::endl;
                const char* responseHeader = "HTTP/1.1 204 No Content\r\n\r\n";
                ssize_t bytesSent = send(clients[i].first.get_fd(), responseHeader, strlen(responseHeader), 0);
                if (bytesSent == -1) {
                    perror("Sending response header failed");
                    FD_CLR(clients[i].first.get_fd(), &io.writeSockets);
                    close(clients[i].first.get_fd());
                    continue;
                }
                std::cout << "sent" << std::endl;
                FD_CLR(clients[i].first.get_fd(), &io.writeSockets);
                close(clients[i].first.get_fd());
                clients.erase(clients.begin() + i);
                i--;
            }
        }
        
    }
    for (std::vector <std::pair <Socket , Server_storage > >::iterator it = _server.begin(); it != _server.end(); ++it)
        close(it->second.getFd());
}

void Multiplexing::handleNewConnection(Socket& serverSocket, Server_storage server)
{
    Client client;
    struct sockaddr_in address = serverSocket.get_address();
    socklen_t clientAddrLen = sizeof(client.get_address());
    int clientSocket = accept(serverSocket.get_fd(), (struct sockaddr *)&address, &clientAddrLen);
    client.set_fd(clientSocket);
    if (clientSocket == -1)
    {
        perror("Accepting connection failed");
    }
    else
    {
        FD_SET(clientSocket, &io.readSockets);
        // FD_SET(clientSocket, &io.writeSockets);
        this->clients.push_back(std::make_pair(client, server));
        maxFd = std::max(maxFd, clientSocket);
        std::cout << "Accepted client connection from " << inet_ntoa(client.get_address().sin_addr) << std::endl;
        std::cout << clients.size() << std::endl;
    }
}