NAME	= webserver

SRC		= 		Webserv.cpp \
				Configue/srcs/Server.cpp \
				Configue/srcs/Location.cpp \
				Configue/srcs/Parser.cpp \
				Configue/srcs/Syntax.cpp \
				Multiplexing/Socket.cpp \
				Multiplexing/Multiplexing.cpp \
				Multiplexing/Client.cpp \
				Request/request.cpp \
				Request/tools.cpp \
				
all: $(NAME)

$(NAME):
	c++ -std=c++98 -Wall -Werror -Wextra $(SRC) -o $(NAME)

clean:
	rm -rf $(NAME)

fclean: clean

re: fclean all