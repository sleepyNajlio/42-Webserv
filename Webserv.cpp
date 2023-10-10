#include "includes/Server.hpp"
int	main(int ac, char **av)
{
	try {
		if (ac != 2)
			throw std::runtime_error("./webserv [filename.config]");
		Server configue_file(av[1]);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return (1);
	}
	return (0);
}