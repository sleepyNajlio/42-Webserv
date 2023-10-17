#include "Configue/includes/Server.hpp"
#include "Configue/includes/Parser.hpp"
#include "Configue/includes/Location.hpp"
#include "Multiplexing/inc/Multiplexing.hpp"
int	main(int ac, char **av)
{
	try {
		if (ac != 2)
			throw std::runtime_error("./webserv [filename.config]");
		Parser configue_file(av[1]);
		Socket serverSocket;
		int i = 1;

		std::vector <Server> it = configue_file.get_servers();
		while(i < configue_file.get_number_of_servers())
		{
    		serverSocket.setupServerSocket(it[i].get_port());
			i++;
		}
		//-------------------------print binded fds

		std::vector <int> temp = serverSocket.get_sockets();
		for (size_t i = 0; i < temp.size(); ++i){
			std::cout << temp[i] << std::endl;}
		
		Multiplexing Multiplexing;
    	Multiplexing.setupServer(serverSocket);
	
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return (1);
	}
	return (0);
}