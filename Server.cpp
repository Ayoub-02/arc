#include "Server.hpp"

std::string Server::getPassword()
{
    return password;
}

bool Server::IsNickTaken(std::string nickname)
{
	for (std::map<int, Client*>::iterator it = clients.begin();it != clients.end();++it)
    {
        if (it->second->getNickname() == nickname)
            return true;
    }
    return false;
}
