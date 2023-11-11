#include "inc/Multiplexing.hpp"

Multiplexing::Multiplexing()
{
    FD_ZERO(&io.readSockets);
    FD_ZERO(&io.writeSockets);
    FD_ZERO(&io.tmpReadSockets);
    FD_ZERO(&io.tmpWriteSockets);
}

Multiplexing::~Multiplexing(){}

ioSets Multiplexing::getIoSets() const
{
    return io;
}

void sendresp(Response &resp)
{
    int rc;
    //send header
    if(resp.head.size() != 0)
    {
        rc = send(resp.fd_sok, resp.head.c_str(), resp.head.size(), 0) ;

        if(resp.bytes_sent < 0)
        {
            resp.clear_client = true;
            return;
        }
        resp.head = "";
    }

    //send the response stored in string 
    else if (resp.response.size() != 0)
    {
        rc = send(resp.fd_sok, resp.response.c_str(), resp.response.size(), 0);
        if (rc <= 0 )
        {
            resp.clear_client = true;
            return;
        }
        resp.bytes_sent += rc;
        resp.response = "";
    }


    // {file case}
    // open file if not opened
    if (!resp.fd_res.is_open() && resp.fd_res_filename.size() != 0)
    {
        resp.fd_res.open(resp.fd_res_filename, std::ios::in | std::ios::binary | std::ios::ate);
        if (!resp.fd_res.is_open())
        {
            resp.clear_client = true;
            return;
        }
        // move the file pointer to the position where it was left (bytes_sent)
        resp.fd_res.seekg(resp.bytes_sent, std::ios::beg);
    }
    //send the response stored in file
    char buffer[2048];
    resp.fd_res.read(buffer, 2048);
    size_t buffer_size = resp.fd_res.gcount();

    if (buffer_size)
    {
		// std::cout << "hna" << std::endl;
            rc = send(resp.fd_sok, buffer, buffer_size, 0);
            if (rc <= 0 )
            {
                resp.clear_client = true;
                return;
            }
            resp.bytes_sent += rc;
        bzero((buffer), 2048);
    }
    // std::cout << resp.bytes_sent << " " << resp.contentTrack << " " << std::endl ;
    resp.fd_res.close();
    //if sending response is done
    if (resp.bytes_sent == resp.contentTrack)
       resp.clear_client = true;
    if (resp.method != "GET" && !resp.contentTrack)
        resp.clear_client = true;
	
}

void Multiplexing::deleteTempFile(Client &client)
{
    if (access(client.req.randomstr.c_str(), F_OK) != -1)
        remove(client.req.randomstr.c_str());
    else
        std::cout << "Error deleting temp file" << std::endl;
}


void Multiplexing::setupServer(std::vector <std::pair <Socket , Server_storage > > _server)
{
    for ( std::vector<std::pair <Socket , Server_storage > >::iterator it = _server.begin(); it != _server.end(); ++it)
    {
        FD_SET(it->second.getFd(), &io.readSockets);
        maxFd = std::max( maxFd, it->second.getFd());
    }
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

        // std::cout << "Number of clients: " << clients.size() << std::endl;
        // loop through clients and check for events
        for (size_t i = 0; i < clients.size(); i++)
        {
            // check for read event
            if (FD_ISSET(clients[i].first.get_fd(), &io.tmpReadSockets))
            {
                
                unsigned char buffer[2048];
                bzero(buffer, 2048);
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
                    // buffer is ready for parse here
                    try{
                        clients[i].first.req.reader(buffer, bytesRead);
                    }
                    catch (std::exception &exp){
                        std::string exceptionMessage = exp.what();
                        clients[i].first.res.set_status_code(std::atoi(exceptionMessage.c_str()));
						// error page call !!
                        clients[i].first.req.setReadDone(true);
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
                clients[i].first.res.fd_sok = clients[i].first.get_fd();
                
                if(!clients[i].first.res.check_res)
                {
                    clients[i].first.res.check_res = true;
                    clients[i].first.res.init_response(clients[i].first.req , clients[i].second);
                }
                if (!clients[i].first.res.clear_client)
                    sendresp(clients[i].first.res);
                if(clients[i].first.res.clear_client)
                {        
                    clients[i].first.res.fd_res.close();
                    FD_CLR(clients[i].first.get_fd(), &io.writeSockets);
                    close(clients[i].first.get_fd());
                    // std::cout << "body fd" << clients[i].first.req.getBody() << "temp file: " << clients[i].first.req.randomstr << std::endl;
                    deleteTempFile(clients[i].first);
                    clients.erase(clients.begin() + i);

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
    struct sockaddr_in address = serverSocket.get_address();
    socklen_t clientAddrLen = sizeof(address);
    int clientSocket = accept(serverSocket.get_fd(), (struct sockaddr *)&address, &clientAddrLen);
    client.set_fd(clientSocket);
    if (clientSocket == -1)
        perror("Accepting connection failed");
    else
    {
        FD_SET(clientSocket, &io.readSockets);
        this->clients.push_back(std::make_pair(client, server));
        maxFd = std::max(maxFd, clientSocket);
    }
}

