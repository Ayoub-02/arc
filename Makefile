NAME = ircserv
SRC = main.cpp Channel.cpp ChannelCommands.cpp Client.cpp Server.cpp UserCommands.cpp UserCommandsHelper.cpp
OBJ = $(SRC:.cpp=.o)
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
HPP = Channel.hpp ChannelCommands.hpp Client.hpp Server.hpp UserCommands.hpp 

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
