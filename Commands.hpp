#pragma once

#include "Server.hpp"
#include "Client.hpp"
#include "channel.hpp"


void    handleJoin(Client* client, std::vector<std::string> params, Server* server);
void    handlePart(Client* client, std::vector<std::string> params,std::string trailing, Server* server);
void    handleTopic(Client *client, std::vector<std::string> params, std::string trailing, Server *server);