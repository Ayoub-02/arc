#include "UserCommands.hpp"

void Server::handleClientCommand(Client& client, const ParsedMessage& cmd) //removed server from parameter 
{
    if (cmd.command == "PASS")
        handlePass(client, cmd, *this);
    else if (cmd.command == "NICK")
        handleNick(client, cmd, *this);
    else if (cmd.command == "USER")
        handleUser(client, cmd);
    else if (cmd.command == "QUIT")
        handleQuit(client, cmd, *this);
    else if(cmd.command == "PRIVMSG")
        handlePrivmsg(client, cmd, *this);
    else if (cmd.command == "PING")                        //bingoooooo added ping 
        handlePing(client, cmd);
}


bool isValidParam(const std::string& param)
{
    if (param.empty())// not empty
        return false;

    if (param.size() > 9)//max 9 letters
        return false;

    for (size_t i = 0; i < param.size(); i++) // doesnt containt special char
	{
		if(!std::isalnum(static_cast<unsigned char>(param[i])) &&
			param[i] != '-' && param[i] != '_' &&
			param[i] != '[' && param[i] != ']' &&
			param[i] != '{' && param[i] != '}' &&
			param[i] != '\\' && param[i] != '|')
			{
				return false;
			}
	}
    return true;
}

void sendToClient(int fd, const std::string& message)
{
    send(fd, message.c_str(), message.size(), 0);
}

bool onlySpaces(std::string trailingMessage)
{
    bool onlySpaces = true;
    for (size_t i = 0; i < trailingMessage.size(); i++)
    {
        if (!isspace(static_cast<unsigned char>(trailingMessage[i])))
        {
            onlySpaces = false;
            break;
        }
    }
    return (onlySpaces);
}

void Server::broadcastQuit(std::string reason, Client &client)
{
    std::string msg = ":" + client.getNickname() + " QUIT :" + reason;

    for (std::map<std::string, Channel*>::iterator it = channels.begin();
         it != channels.end(); ++it)
    {
        Channel* ch = it->second;

        if (ch->ismember(&client))
            ch->broadcast(msg, &client);
    }
}
