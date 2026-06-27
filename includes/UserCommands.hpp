#pragma once
#include <string>
#include <vector>
#include <sys/socket.h>
#include "Server.hpp"
#include "Channel.hpp"


class Client;
class Server;
class Channel;
struct ParsedMessage;

void handleNick(Client& client, const ParsedMessage& cmd, Server& server);
void handlePass(Client& client, const ParsedMessage& cmd, Server& server);
void handleUser(Client& client, const ParsedMessage& cmd);
void handleQuit(Client& client, const ParsedMessage& cmd, Server& server);
void handlePrivmsg(Client& client, const ParsedMessage& cmd, Server& server);
void handlePing(Client& client, const ParsedMessage& cmd);
void sendToClient(int fd, const std::string& message);
bool isValidParam(const std::string& param);
bool onlySpaces(std::string trailingMessage);
