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
				request/request.cpp \
				request/tools.cpp

OBJ				=	${SRC:%.cpp=%.o}

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