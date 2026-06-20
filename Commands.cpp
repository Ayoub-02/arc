#include "Client.hpp"
#include "channel.hpp"
#include "Server.hpp"

void    handleJoin(Client* client, std::vector<std::string> params, Server* server)
{
    //validate

    if (!client->getIsRegistered())
    {
        std::string msg = "451 " + client->getNickname() + " :You have not registered\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return ;
    }
    if (params.empty())
    {
        std::string msg = "461 " + client->getNickname() + " JOIN :Not enough parameters\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return ;
    }
    std::string channelName = params[0];
    std::string password = (params.size() > 1) ? params[1] : "";
    if (channelName[0] != '#')
    {
        std::string msg = "476 " + client->getNickname() + " " + channelName + " :Bad Channel Mask\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    std::map<std::string, Channel*>& channels = server->getChannels();
    bool channelExist = (channels.find(channelName) != channels.end());

    //start executing with some checks

    Channel *channel;
    if (!channelExist)
    {
        channel = new Channel(channelName);
        channels[channelName] = channel;
    }
    else 
    {
        channel = channels[channelName];
        if (!channel->getpassword().empty() && channel->getpassword() != password)
        {
            std::string msg = "475 " + client->getNickname() + " " + channelName + " :Cannot join channel (+k)\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }
        int limit = channel->getuserlimit();
        if (limit > 0 && (int)channel->getmembers().size() > limit)
        {
            std::string msg = "471 " + client->getNickname() + " " + channelName + " :Cannot join channel (+l)\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }
        channel->addmember(client);
        if (!channelExist)
            channel->addoperator(client);
        std::string joinMsg = ":" + client->getPrefix() + " JOIN " + channelName + "\r\n";
        send(client->getFd(), joinMsg.c_str(), joinMsg.size(), 0);
        channel->broadcast(":" + client->getPrefix() + " JOIN " + channelName, client);
        if (!channel->gettopic().empty())
        {
            std::string topicMsg = "332 " + client->getNickname() + " " + channelName + " :" + channel->gettopic() + "\r\n";
            send(client->getFd(), topicMsg.c_str(), topicMsg.size(), 0);
        }
        std::string names = "353 " + client->getNickname() + " = " + channelName + " :";
        std::vector<Client*> members = channel->getmembers();
        size_t i = 0;
        while (i < members.size())
        {
            names += members[i]->getNickname();
            i++;
        }
        names+="\r\n";
        send(client->getFd(), names.c_str(), names.size(), 0);
        std::string endNames = "366 " + client->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
        send(client->getFd(), endNames.c_str(), endNames.size(), 0);
    }
};