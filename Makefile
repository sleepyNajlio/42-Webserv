NAME			= 	webserv
CXX			=	c++
CPPFLAGS		=	-Wall -Wextra -Werror
SRC			=	Webserv.cpp Configue/srcs/Server.cpp Multiplexing/Socket.cpp Multiplexing/Multiplexing.cpp  Multiplexing/Client.cpp
OBJ			=	${SRC:%.cpp=%.o}

all : $(NAME)

%.o : %.cpp
		$(CXX) $(CPPFLAGS) -c $< -o $@

$(NAME) : $(OBJ)
	$(CXX) $(CPPFLAGS)  $(SRC) -o $(NAME)

clean :
	rm -rf $(OBJ) 

fclean : clean
	rm -rf  $(NAME) 
re : fclean all 