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

void Multiplexing::WriteData(std::pair < Client , Server_storage > &client)
{
    char buffer[2048];
    bzero(buffer, 2048);
    if (!client.first.res.getFileIsOpen())
    {
        client.first.res.open_file(client.second, client.first.res.getFd_res_filename());
    }

}


void Multiplexing::setupServer(std::vector <std::pair <Socket , Server_storage > > &_server)
{
    for ( std::vector<std::pair <Socket , Server_storage > >::iterator it = _server.begin(); it != _server.end(); ++it)
    {
        FD_SET(it->second.getFd(), &io.readSockets);
        maxFd = std::max( maxFd, it->second.getFd());
    }

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
        for (std::vector <std::pair <Socket , Server_storage > >::iterator it = _server.begin(); it != _server.end(); ++it)
        {
            if (FD_ISSET(it->second.getFd(), &io.tmpReadSockets))
            {
                handleNewConnection(it->first, it->second);
            }
        }
        // loop through clients and check for events
        for (size_t i = 0; i < clients.size(); i++)
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
                    std::cout << "Received: " << buffer << std::endl;
                    // buffer is ready for parse here
                    try{
                        clients[i].first.req.reader(buffer, bytesRead);
                    }
                    catch (std::exception &exp){
                        std::string exceptionMessage = exp.what();
                        clients[i].first.res.set_status_code(std::stoi(exceptionMessage));
						// error page call !!
                    }
                    // move from read to write sockets if request is done 
                    if (clients[i].first.req.isReadDone())
                    {
                        
                        FD_CLR(clients[i].first.get_fd(), &io.readSockets);
                        FD_SET(clients[i].first.get_fd(), &io.writeSockets);
                    }
                }
            }
            // check for write event
            if (FD_ISSET(clients[i].first.get_fd(), &io.tmpWriteSockets))
            {

                if (clients[i].first.res.get_status_code() == 0)
                    clients[i].first.res.init_response(clients[i].first.req , clients[i].second);
                WriteData(&clients[i]);
                if (clients[i].first.res.isWriteDone())
                {
                    std::cout << "write done" << std::endl;
                    FD_CLR(clients[i].first.get_fd(), &io.writeSockets);
                    // if (clients[i].first.res.get_fd() > 0)
                    //     close(clients[i].first.res.get_fd());
                    clients.erase(clients.begin() + i);
                    i--;
                }

                // clients[i].first.res.fd_sok = clients[i].first.get_fd();
                // if(!clients[i].first.res.check_res)
                // {
                //     clients[i].first.res.check_res = true;
                //     clients[i].first.res.init_response(clients[i].first.req , clients[i].second);
                // }
                // if (!clients[i].first.res.clear_client)
                //     clients[i].first.res.ft_sendResponse();
                // ft_response(clients[i].first, clients[i].second);
                // std::cout << "write" << std::endl;
                // const char* responseHeader = "HTTP/1.1 204 No Content\r\n\r\n";
                // std::string response = clients[i].first.res.get_response();
                // if (send(clients[i].first.get_fd(), response.c_str() , response.size(), 0) < 1 )
                // {
                //     perror("Sending response header failed");
                //     FD_CLR(clients[i].first.get_fd(), &io.writeSockets);
                //     close(clients[i].first.get_fd()); 
                //     continue;
                // }

                if(clients[i].first.res.clear_client)
                {
                     for (size_t i = 0; i < clients.size(); i++)
                    {
                        std::cout << "-->" << clients[i].first.res.fd_res.width() << std::endl;
                        std::cout << "clients[" << i << "].first.get_fd() = " << clients[i].first.get_fd() << std::endl;
                    }
                    std::cout << "clear client  url :::: "<< clients[i].first.req.getUrl() << std::endl;          
                    clients[i].first.res.fd_res.close();
                    FD_CLR(clients[i].first.get_fd(), &io.writeSockets);
                    close(clients[i].first.get_fd());
                    clients.erase(clients.begin() + i);
                    //print vector clients
                   for (size_t i = 0; i < clients.size(); i++)
                    {
                        std::cout << "**-->" << clients[i].first.res.fd_res.width() << std::endl;
                        std::cout << "clients[" << i << "].first.get_fd() = " << clients[i].first.get_fd() << std::endl;
                        std::cout << "clients[" << i << "].first.get_fd() ===== PATH " << clients[i].first.req.getUrl()
                         << std::endl;

                    }

                    i--;
                }
            }
        }
    }
    for (std::vector <std::pair <Socket , Server_storage > >::iterator it = _server.begin(); it != _server.end(); ++it)
        close(it->second.getFd());
}

void Multiplexing::handleNewConnection(Socket& serverSocket, Server_storage server)
{
    Client client;


    client.res.clear_client = false;
    client.res.check_res = false;
    //client.res.i = 0;
    // std::cout << client.req.getChunkSize() << std::endl;
    struct sockaddr_in address = serverSocket.get_address();
    socklen_t clientAddrLen = sizeof(address);
    int clientSocket = accept(serverSocket.get_fd(), (struct sockaddr *)&address, &clientAddrLen);
    client.set_fd(clientSocket);
    std::cout << "handleNewConnection\n" << client.get_fd() << " ----->\n " << client.req.getUrl()<< "===>\n" << client.res.check_res << std::endl;
    if (clientSocket == -1)
    {
        perror("Accepting connection failed");
    }
    else
    {
        FD_SET(clientSocket, &io.readSockets);
        this->clients.push_back(std::make_pair(client, server));
        maxFd = std::max(maxFd, clientSocket);
        // std::cout << "Accepted client connection from " << inet_ntoa(client.get_address().sin_addr) << std::endl;
        // std::cout << clients.size() << std::endl;
    }
}

