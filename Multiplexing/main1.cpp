#include "inc/Multiplexing.hpp"
#include <signal.h>

int main(int ac, char **av)
{
    Socket serverSocket;
    serverSocket.setupServerSocket(atoi(av[1]));

    signal(SIGPIPE, SIG_IGN);
    Multiplexing Multiplexing;
    std::vector<std::pair<Socket, Server_storage>> sockets;
    sockets.push_back(std::make_pair(serverSocket, Server_storage()));
    Multiplexing.setupServer(sockets);

    return 0;
}