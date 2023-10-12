NAME	= webserver

SRC		= main1.cpp Socket.cpp Multiplexing.cpp request.cpp Client.cpp

all: $(NAME)

$(NAME):
	c++ -std=c++98 $(SRC) -o $(NAME)

clean:
	rm -rf $(NAME)

fclean: clean

re: fclean all