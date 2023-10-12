#include "inc/Multiplexing.hpp"

Multiplexing::Multiplexing()
{
    FD_ZERO(&io.readSockets);
    FD_ZERO(&io.writeSockets);
    FD_ZERO(&io.tmpReadSockets);
    FD_ZERO(&io.tmpWriteSockets);
}

Multiplexing::~Multiplexing()
{
}

ioSets Multiplexing::getIoSets() const
{
    return io;
}


void Multiplexing::setupServer(Socket& serverSocket)
{
    FD_SET(serverSocket.get_fd(), &io.readSockets);
    maxFd = serverSocket.get_fd();

    //==> I COMMENT THIS FOR COMPILATION SAKE

    //const char* responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: video/mp4\r\nContent-Length: ";
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
        // check for new connection
        if (FD_ISSET(serverSocket.get_fd(), &io.tmpReadSockets))
        {
            handleNewConnection(serverSocket);
        }
        // loop through clients and check for events
        for (size_t i = 0; i < clients.size(); i++)
        {
            // check for read event
            if (FD_ISSET(clients[i].get_fd(), &io.tmpReadSockets))
            {
                
                unsigned char buffer[2048];
                bzero(buffer, 2048);
                ssize_t bytesRead = recv(clients[i].get_fd(), buffer, sizeof(buffer), 0);
                if (bytesRead == -1) {
                    perror("Receiving data failed");
                    break;
                }
                else if (bytesRead == 0) {
                    std::cout << "Client closed connection" << std::endl;
                    close(clients[i].get_fd());
                    FD_CLR(clients[i].get_fd(), &io.readSockets);
                    FD_CLR(clients[i].get_fd(), &io.writeSockets);
                    clients.erase(clients.begin() + i);
                    break;
                }
                else {
                    buffer[bytesRead] = '\0';
                    // buffer is ready for parse here

                    //==> I COMMENT THIS FOR COMPILATION SAKE

                    // check if request reading is done
                    // if (clients[i].get_request().read_done)
                    // {
                    //     FD_CLR(clients[i].get_fd(), &io.readSockets);
                    //     FD_SET(clients[i].get_fd(), &io.writeSockets);
                    // }
                }
            }
            // check for write event
            if (FD_ISSET(clients[i].get_fd(), &io.tmpWriteSockets))
            {
                std::cout << "write" << std::endl;
                const char* responseHeader = "HTTP/1.1 204 No Content\r\n\r\n";
                ssize_t bytesSent = send(clients[i].get_fd(), responseHeader, strlen(responseHeader), 0);
                if (bytesSent == -1) {
                    perror("Sending response header failed");
                    FD_CLR(clients[i].get_fd(), &io.writeSockets);
                    close(clients[i].get_fd());
                    continue;
                }
                std::cout << "sent" << std::endl;
                FD_CLR(clients[i].get_fd(), &io.writeSockets);
                close(clients[i].get_fd());
                clients.erase(clients.begin() + i);
                i--;
            }
        }
        
    }
    close(serverSocket.get_fd());
}

void Multiplexing::handleNewConnection(Socket& serverSocket)
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
        this->clients.push_back(client);
        maxFd = std::max(maxFd, clientSocket);
        std::cout << "Accepted client connection from " << inet_ntoa(client.get_address().sin_addr) << std::endl;
        std::cout << clients.size() << std::endl;
    }
}