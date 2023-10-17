#include "Configue/includes/Server.hpp"
#include "Configue/includes/Location.hpp"
#include "Configue/includes/Parser.hpp"
int	main(int ac, char **av)
{
	try {
		if (ac != 2)
			throw std::runtime_error("./webserv [filename.config]");
		Parser configue(av[1]);

		//-------------------------PRINT SERVERS
		std::cout << configue.get_servers() << std::endl;
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return (1);
	}
	return (0);
}