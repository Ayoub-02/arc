NAME =	ircserv
SRC =	srcs/main.cpp srcs/channel/Channel.cpp srcs/channel/ChannelCommands.cpp \
		srcs/client/Client.cpp srcs/server/Server.cpp srcs/client/UserCommands.cpp \
		srcs/client/UserCommandsHelper.cpp

OBJ =	$(SRC:.cpp=.o)
CXX =	c++
CXXFLAGS =	-Wall -Wextra -Werror -std=c++98
HPP =	includes/Channel.hpp includes/ChannelCommands.hpp includes/Client.hpp includes/Server.hpp includes/UserCommands.hpp 

all: $(NAME)

$(NAME): $(OBJ) $(HPP)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean 
