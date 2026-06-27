#include "../../includes/UserCommands.hpp"

void handlePass(Client& client, const ParsedMessage& cmd, Server& server)
{
    if (client.getIsRegistered())
    {
        sendToClient(client.getFd(), "462 ERR_ALREADYREGISTRED :You may not reregister\r\n");
        return;
    }
    if (cmd.params.size() != 1)
    {
        sendToClient(client.getFd(), "461 ERR_NEEDMOREPARAMS PASS :Not enough parameters\r\n");
        return;
    }
    if (cmd.params[0] != server.getServerPassword())
    {
        sendToClient(client.getFd(), "464 ERR_PASSWDMISMATCH :Password incorrect\r\n");
        return;
    }

    client.setIsAuthenticated(true);
}

void handleNick(Client& client, const ParsedMessage& cmd, Server& server)
{
    if (!client.getIsAuthenticated())
    {
        sendToClient(client.getFd(), "451 ERR_NOTREGISTERED :You have not registered\r\n");
        return;
    }
    if (cmd.params.size() != 1) 
    {
        sendToClient(client.getFd(), "431 ERR_NONICKNAMEGIVEN :No nickname given\r\n");
        return;
    }
    if (!isValidParam(cmd.params[0]))
    {
        sendToClient(client.getFd(), "432 ERR_ERRONEUSNICKNAME :Invalid nickname\r\n");
        return;
    }
    if (server.isNickTaken(cmd.params[0])) 
    {
        sendToClient(client.getFd(), "433 ERR_NICKNAMEINUSE :Nickname is already in use\r\n");
        return;
    }
    
    std::string oldNick = client.getNickname();
    client.setNickname(cmd.params[0]);
    if (!client.getNickname().empty() && !client.getUsername().empty())
        client.setIsRegistered(true);


    std::string msg = oldNick + " changed his nickname to :" + client.getNickname();
    for (std::map<std::string, Channel*>::iterator it = server.getChannels().begin();
         it != server.getChannels().end(); ++it)
    {
        Channel* ch = it->second;
        if (ch->ismember(&client))
            ch->broadcast(msg, &client);
    }
}

void handleUser(Client& client, const ParsedMessage& cmd)
{
    if (!client.getIsAuthenticated())
    {
        sendToClient(client.getFd(), "451 ERR_NOTREGISTERED :You have not registered\r\n");
        return;
    }
    if (client.getIsRegistered())
    {
        sendToClient(client.getFd(), "462 ERR_ALREADYREGISTRED :You may not reregister\r\n");
        return;
    }
    if (cmd.params.size() < 3 || cmd.trailing.empty())
    {
        sendToClient(client.getFd(), "461 ERR_NEEDMOREPARAMS USER :Not enough parameters\r\n");
        return;
    }
    if (cmd.params[0].empty())
    {
        sendToClient(client.getFd(), "461 ERR_NEEDMOREPARAMS USER :Invalid username\r\n");
        return;
    }
    if (onlySpaces(cmd.trailing))
    {
        sendToClient(client.getFd(), "461 ERR_NEEDMOREPARAMS USER :Invalid realname\r\n");
        return;
    }
    client.setUsername(cmd.params[0]);
    client.setHostname(cmd.params[1]);
    client.setServername(cmd.params[2]);
    client.setRealname(cmd.trailing);

    if (!client.getNickname().empty() && !client.getUsername().empty())
        client.setIsRegistered(true);
}

void handleQuit(Client& client, const ParsedMessage& cmd, Server& server)
{
    std::string reason;
    if (!cmd.trailing.empty())
        reason = cmd.trailing;
    else if (!cmd.params.empty())
        reason = cmd.params[0];
    else 
        reason = "Without reason!";
    
    server.broadcastQuit(reason, client);
    server.removeMemberFromAllChannels(client);
    server.disconnectClient(client.getFd());
}

void handlePrivmsg(Client& client, const ParsedMessage& cmd, Server& server)
{
    if (!client.getIsRegistered())
    {
        sendToClient(client.getFd(), "451 ERR_NOTREGISTERED :You have not registered\r\n");
        return;
    }
    if (cmd.params.empty() || cmd.params[0].empty())
    {
        sendToClient(client.getFd(), "411 ERR_NORECIPIENT :No recipient given\r\n");
        return;
    }
    if (cmd.trailing.empty())
    {
        sendToClient(client.getFd(), "412 ERR_NOTEXTTOSEND :No text to send\r\n");
        return;
    }
    std::string target = cmd.params[0];
    const std::string  message = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + cmd.trailing;

    if (target[0] == '#' || target[0] == '&')
    {
        if (!server.channelExistence(target))
        {
            sendToClient(client.getFd(), "403 ERR_NOSUCHCHANNEL :No such channel\r\n");
            return;
        }
        server.transferMessage(target, message, client);
    }
    else
    {
        if (!server.clientExistence(target))
        {
            sendToClient(client.getFd(), "401 ERR_NOSUCHNICK :No such nick/channel\r\n");
            return;
        }
        server.transferMessage(target, message + "\r\n", client);
    }
}

void handlePing(Client& client, const ParsedMessage& cmd)   
{
    if ((cmd.params.empty() || cmd.params[0].empty()) && cmd.trailing.empty())
    {
        sendToClient(client.getFd(), "409 ERR_NOORIGIN :No origin specified\r\n");
        return;
    }
    std::string key;
    if (!cmd.trailing.empty())
        key = cmd.trailing;
    else
        key = cmd.params[0];

    sendToClient(client.getFd(), "PONG :" + key + "\r\n");
}
