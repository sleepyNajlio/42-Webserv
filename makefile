NAME	= webserver

SRC		= 		Webserv.cpp \
				Conf/conf_parser.cpp Conf/conf_reader.cpp Conf/tools.cpp Conf/tools1.cpp \Conf/location_storage.cpp Conf/server_storage.cpp \
				Multiplexing/Socket.cpp Multiplexing/Multiplexing.cpp Multiplexing/Client.cpp \
				request/request.cpp request/tools.cpp response/response.cpp \

				
all: $(NAME)

$(NAME):
	c++  -Wall -Werror -Wextra $(SRC) -o $(NAME)

clean:
	rm -rf $(NAME)

fclean: clean

re: fclean all