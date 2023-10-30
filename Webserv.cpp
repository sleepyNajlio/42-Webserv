#include "Multiplexing/inc/Multiplexing.hpp"
#include "Conf/incs/conf_parser.hpp"

int	main(int ac, char **av)
{
	Conf_parser _parser;
    Socket serverSocket;
    std::vector < Server_storage > _servers;
    std::vector <std::pair <Socket , Server_storage > > serverList;
    Multiplexing Multiplexing;
    
    parseServer(ac, av, _parser);

    _servers = _parser.getServers();

    serverSocket.setupServerSocket(_servers, serverList);
    Multiplexing.setupServer(serverList);
	return (0);
}