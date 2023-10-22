#include "inc/Multiplexing.hpp"

int main(int ac, char **av)
{
    Socket serverSocket;
    serverSocket.setupServerSocket(atoi(av[1]));

    Multiplexing Multiplexing;
    Multiplexing.setupServer(serverSocket);

    return 0;
}