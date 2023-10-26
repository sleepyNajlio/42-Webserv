NAME	= webserver

SRC		= 		Webserv.cpp \
				Conf/conf_parser.cpp Conf/conf_reader.cpp Conf/tools.cpp Conf/tools1.cpp \Conf/location_storage.cpp Conf/server_storage.cpp \
				Multiplexing/Socket.cpp Multiplexing/Multiplexing.cpp Multiplexing/Client.cpp \
				Request/request.cpp Request/tools.cpp 

				
all: $(NAME)

$(NAME):
	c++ -std=c++98 -Wall -Werror -Wextra $(SRC) -o $(NAME)

clean:
	rm -rf $(NAME)

fclean: clean

re: fclean all