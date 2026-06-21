#ifndef COMMAND_PARSER_HPP
# define COMMAND_PARSER_HPP
#include <string>
#include <vector>
#include <sys/socket.h>
#include "Server.hpp"
#include "channel.hpp"


class Client;
class Server;
struct ParsedMessage;

void handleNick(Client& client, const ParsedMessage& cmd, Server& server);
void handlePass(Client& client, const ParsedMessage& cmd, Server& server);
void handleUser(Client& client, const ParsedMessage& cmd, Server& server);
void handleQuit(Client& client, const ParsedMessage& cmd, Server& server);

void sendToClient(int fd, const std::string& message);

bool isValidParam(const std::string& param);
bool onlySpaces(std::string trailingMessage);

#endif