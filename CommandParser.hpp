#ifndef COMMAND_PARSER_HPP
# define COMMAND_PARSER_HPP
#include <string>
#include <vector>
#include <sys/socket.h>
#include "Server.hpp"
#include "channel.hpp"



struct Command
{
    std::string command_name ;
    std::vector<std::string> params;
    std::string trailingMessage;
};

void handleNick(Client& client, Command& cmd, Server& server);
void handlePass(Client& client, Command& cmd, Server& server);
void handleUser(Client& client, Command& cmd, Server& server);
void handleQuit(Client& client, Command& cmd, Server& server);

void sendToClient(int fd, const std::string& message);

bool isValidParam(const std::string& param);
bool onlySpaces(std::string trailingMessage);

#endif