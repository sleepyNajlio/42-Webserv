CPP = g++

CPPFLAGS = -std=c++98 -Wall

TARGET = req

SRCS = main.cpp request.cpp tools.cpp

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CPP) $(CPPFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

.PHONY: all clean fclean

all: $(TARGET)

