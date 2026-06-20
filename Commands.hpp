#pragma once

#include "Server.hpp"
#include "Client.hpp"
#include "channel.hpp"


void    handleJoin(Client* client, std::vector<std::string> params, Server* server);