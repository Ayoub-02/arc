#include "CommandParser.hpp"


void handlePass(Client& client, Command& cmd, Server& server)
{
    if (client.getIsRegistered()) //is he already registred
    {
        sendToClient(client.getFd(), "462 ERR_ALREADYREGISTRED :You may not reregister\r\n");
        return;
    }
    if (cmd.params.size() != 1) // he wrote only pass
    {
        sendToClient(client.getFd(), "461 ERR_NEEDMOREPARAMS PASS :Not enough parameters\r\n");
        return;
    }
    if (cmd.params[0] != server.getServerPassword())  //pass incorrect
    {
        sendToClient(client.getFd(), "464 ERR_PASSWDMISMATCH :Password incorrect\r\n");
        return;
    }

    client.setIsAuthenticated(true); // ya3ni password s7i7 but no registration yet
}

void handleNick(Client& client, Command& cmd, Server& server)   //li mora : kayt igonara this is valid -> NICK mehdi :mehdi : hufdsj
{
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
    client.setNickname(cmd.params[0]);
}

                                                             // USER   "mehdi"    belk      dfsfds :mehdi elk
void handleUser(Client& client, Command& cmd, Server& server) //USER <username> <hostname> <servername> :<realname>
{
    if (!client.getIsAuthenticated())
    {
        sendToClient(client.getFd(), "451 ERR_NOTREGISTERED :You have not registered\r\n");
        return;
    }
    if (client.getIsRegistered())
    {
        sendToClient(client.getFd(), "462 ERR_ALREADYREGISTRED :\r\n"); //to look up
        return;
    }
    if (cmd.params.size() < 3 || cmd.trailingMessage.empty())
    {
        sendToClient(client.getFd(), "461 ERR_NEEDMOREPARAMS USER :Not enough parameters\r\n");
        return;
    }
    if (cmd.params[0].size() == 0 || (cmd.trailingMessage.size() == 0 || isspace(cmd.trailingMessage[0])))  //username and realname should not be empty
    {
        sendToClient(client.getFd(), "461 ERR_NEEDMOREPARAMS USER :Not enough parameters\r\n");
        return;
    }
    client.setUsername(cmd.params[0]);
    client.setHostname(cmd.params[1]);
    client.setServername(cmd.params[2]);
    client.setRealname(cmd.trailingMessage);

    if (client.getIsAuthenticated() && client.getNickname().size() != 0 
        && client.getUsername().size() != 0)
        {
            client.setIsRegistered(true);
        }
}

void handleUser(Client& client, Command& cmd, Server& server)
{
    if (!client.getIsAuthenticated())
    {
        sendToClient(client.getFd(),
            "451 ERR_NOTREGISTERED :You have not registered\r\n");
        return;
    }
    if (client.getIsRegistered())
    {
        sendToClient(client.getFd(),
            "462 ERR_ALREADYREGISTRED :You may not reregister\r\n");
        return;
    }
    if (cmd.params.size() < 3 || cmd.trailingMessage.empty())
    {
        sendToClient(client.getFd(),
            "461 ERR_NEEDMOREPARAMS USER :Not enough parameters\r\n");
        return;
    }
    if (cmd.params[0].empty())
    {
        sendToClient(client.getFd(),
            "461 ERR_NEEDMOREPARAMS USER :Invalid username\r\n");
        return;
    }
    if (onlySpaces(cmd.trailingMessage))
    {
        sendToClient(client.getFd(),
            "461 ERR_NEEDMOREPARAMS USER :Invalid realname\r\n");
        return;
    }
    client.setUsername(cmd.params[0]);
    client.setHostname(cmd.params[1]);
    client.setServername(cmd.params[2]);
    client.setRealname(cmd.trailingMessage);

    if (client.getIsAuthenticated()
        && !client.getNickname().empty()
        && !client.getUsername().empty())
    {
        client.setIsRegistered(true);
    }
}

void handleQuit(Client& client, Command& cmd, Server& server)
{
    std::string reason;
    if (!cmd.trailingMessage.empty())
        reason = cmd.trailingMessage;
    else if (!cmd.params.empty())
        reason = cmd.params[0];
    
    // server.broadcastQuit(&client, reason); send message when dying to all members that exist in his joined channels
    // server.removeMemberFromAllChannels(client);
    server.disconnectClient(client.getFd());

    

    
    


}

