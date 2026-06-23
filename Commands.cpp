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
        int limit = channel->getuserlimit();
        if (!channel->getpassword().empty() && channel->getpassword() != password)
        {
            std::string msg = "475 " + client->getNickname() + " " + channelName + " :Cannot join channel (+k)\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }
        if (limit > 0 && (int)channel->getmembers().size() >= limit)
        {
            std::string msg = "471 " + client->getNickname() + " " + channelName + " :Cannot join channel (+l)\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }    
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
};


void    handlePart(Client* client, std::vector<std::string> params, std::string trailing, Server* server)
{
    if (!client->getIsRegistered())
    {
        std::string msg = "451 " + client->getNickname() + " :You have not registered\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (params.empty())
    {
        std::string msg = "461 " + client->getNickname() + " PART :Not enough parameters\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    std::string channelName = params[0];
    std::map<std::string, Channel*>& channels = server->getChannels();

    if (channels.find(channelName) == channels.end())
    {
        std::string msg = "403 " + client->getNickname() + " " + channelName + " :No such channel\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return ;
    }
    Channel *channel = channels[channelName];

    if (!channel->ismember(client))
    {
        std::string msg = "442" + client->getNickname() + " " +  channelName + " :You're not on that channel\r\n";
        send(client->getFd() , msg.c_str() , msg.size() , 0);
        return ;
    }
    std::string partMsg = ":" + client->getPrefix() + " PART " + channelName + "\r\n";
    send(client->getFd() , partMsg.c_str() , partMsg.size(), 0);
    channel->broadcast(":" + client->getPrefix() + " PART " + channelName, client);

    channel->removememeber(client);
    if (channel->getmembers().empty())
    {
        channels.erase(channelName);
        delete channel;
    }
};

void    handleTopic(Client *client, std::vector<std::string> params, std::string trailing, Server *server)
{
    if (!client->getIsRegistered())
    {
        std::string msg = "451 " + client->getNickname() + " :You have not registered\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return ;
    }
    if (params.empty())
    {
        std::string msg = "461 " + client->getNickname() + " TOPIC :Not enough parameters\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    std::string channelName = params[0];
    std::map<std::string, Channel*> &channels = server->getChannels();
    if (channels.find(channelName) == channels.end())
    {
        std::string msg = "442 " + client->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        send(client->getFd() , msg.c_str(), msg.size(), 0);
        return ;
    }
    Channel *channel = channels[channelName];
    if (!channel->ismember(client))
    {
        std::string msg = "442 " + client->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        send(client->getFd() , msg.c_str(), msg.size(), 0);
        return ;
    }
    //VIEW TOPIC CASE
    if (trailing.empty())
    {
        if (channel->gettopic().empty())
        {
            std::string msg = "331 " + client->getNickname() + " " + channelName + " :No topic is set\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
        }
        else
        {
            std::string msg = "332 " + client->getNickname() + " " + channelName + " :" + channel->gettopic() + "\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
        }
        return ;
    }

    // SET TOPIC
    if (channel->istopicrestricted() && !channel->isoperator(client))
    {
        std::string msg = "482 " + client->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    channel->settopic(trailing);
    std::string topicMsg = ":" + client->getPrefix() + " TOPIC " + channelName + " :" + trailing + "\r\n";
    send(client->getFd(), topicMsg.c_str(), topicMsg.size(), 0);
    channel->broadcast(":" + client->getPrefix() + " TOPIC " + channelName + " :" + trailing, client);
};


void    handleInvite(Client *client , std::vector<std::string> params, Server *server)
{
    if (!client->getIsRegistered())
    {
        std::string msg = "451 " + client->getNickname() + " :You have not registered\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (params.size() < 2)
    {
        std::string msg = "461 " + client->getNickname() + " INVITE :Not enough parameters\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    std::string target = params[0];// nickname of the invited
    std::string channelName = params[1];
    std::map<std::string, Channel*> &channels = server->getChannels();
    if (channels.find(channelName) == channels.end())
    {
        std::string msg = "403 " + client->getNickname() + " " + channelName + " :No such channel\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    Channel *channel = channels[channelName];
    if (!channel->ismember(client))
    {
        std::string msg = "442 " + client->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    Client *targetClient = server->getClientNickName(target);
    if (!targetClient)
    {
        std::string msg = "401 " + client->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (!channel->ismember(targetClient))
    {
        std::string msg = "443 " + client->getNickname() + " " + target + " " + channelName + " :is already on channel\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (channel->isinviteonly() && !channel->isoperator(client))
    {
        std::string msg = "482 " + client->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    channel->addInvite(targetClient);
    //send a notice to the invited
    std::string inviteNotice = ":" + client->getPrefix() + " INVITE " + target + " :" + channelName + "\r\n";
    send(targetClient->getFd(), inviteNotice.c_str(), inviteNotice.size(), 0);
    
    //send a notice for the inviter
    std::string reply = "341 " + client->getNickname() + " " + target + " " + channelName + "\r\n";
    send(client->getFd(), reply.c_str(), reply.size(), 0);
};
