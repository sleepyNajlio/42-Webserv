NAME			= 	webserv
CXX				=	c++
CPPFLAGS		=	-Wall -Wextra -Werror
SRC			=	Webserv.cpp \
				Configue/srcs/Server.cpp \
				Configue/srcs/Location.cpp \
				Configue/srcs/Parser.cpp \
				Configue/srcs/Syntax.cpp \
				Multiplexing/Socket.cpp \
				Multiplexing/Multiplexing.cpp \
				Multiplexing/Client.cpp \
				Request/request.cpp \
				Request/tools.cpp \

OBJ				=	${SRC:%.cpp=%.o}

all : $(NAME) clean

%.o : %.cpp
		$(CXX) -c $< -o $@

$(NAME) : $(OBJ)
	$(CXX)   $(SRC) -o $(NAME)

clean :
	rm -rf $(OBJ) 

fclean : clean
	rm -rf  $(NAME) 
re : fclean all 