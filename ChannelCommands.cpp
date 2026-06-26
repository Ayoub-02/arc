#include "Client.hpp"
#include "Channel.hpp"
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
    if (channelName[0] != '#' && channelName[0] != '&')
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
        if (channel->isinviteonly() && !channel->isInvited(client))
        {
            std::string msg = "473 " + client->getNickname() + " " + channelName + " :Cannot join channel (+i)\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }
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
    if (channel->ismember(client))
    {
        std::string msg = ":localhost NOTICE " + client->getNickname() + " :You are already a member of " + channelName + "\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    channel->addmember(client);
    if (channel->isInvited(client))
        channel->removeInvite(client);
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
    std::string finalName;
    while (i < members.size())
    {
        if (channel->isoperator(members[i]))
            finalName = "@" + members[i]->getNickname();
        else
            finalName = members[i]->getNickname();
        names += finalName + " ";
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
        std::string msg = "442 " + client->getNickname() + " " +  channelName + " :You're not on that channel\r\n";
        send(client->getFd() , msg.c_str() , msg.size() , 0);
        return ;
    }
    std::string rawMsg = ":" + client->getPrefix() + " PART " + channelName;
    if (!trailing.empty())
        rawMsg += " :" + trailing;
    std::string partMsg = rawMsg;
    partMsg += "\r\n";
    send(client->getFd() , partMsg.c_str() , partMsg.size(), 0);
    channel->broadcast(rawMsg , client);

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
        std::string msg = "403 " + client->getNickname() + " " + channelName + " :No such channel\r\n";
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
        std::string msg = "401 " + client->getNickname() + " " + target + " :No such nick/channel\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (channel->ismember(targetClient))
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


void    handleKick(Client* client, std::vector<std::string> params, std::string trailing, Server* server)
{
    if (!client->getIsRegistered())
    {
        std::string msg = "451 " + client->getNickname() + " :You have not registered\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (params.size() < 2)
    {
        std::string msg = "461 " + client->getNickname() + " KICK :Not enough parameters\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    std::map<std::string, Channel*> &channels = server->getChannels();
    std::string target = params[1];
    std::string channelName = params[0];
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
    if (!channel->isoperator(client))
    {
        std::string msg = "482 " + client->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    Client *targetClient = server->getClientNickName(target);
    if (!targetClient)
    {
        std::string msg = "401 " + client->getNickname() + " " + target + " :No such nick\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (!channel->ismember(targetClient))
    {
        std::string msg = "441 " + client->getNickname() + " " + target + " " + channelName + " :They aren't on that channel\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    std::string rawMsg = ":" + client->getPrefix() + " KICK " + channelName + " " + target;
    if (!trailing.empty())
        rawMsg += (" :" + trailing);
    std::string Kickmsg = rawMsg;
    Kickmsg += "\r\n";
    send(targetClient->getFd(), Kickmsg.c_str(), Kickmsg.size(), 0);
    channel->broadcast(rawMsg, targetClient);
    channel->removememeber(targetClient);
    if (channel->getmembers().empty())
    {
        channels.erase(channelName);
        delete channel;
    }
};

void    handleMode(Client* client, std::vector<std::string> params, Server* server)
{
    if (!client->getIsRegistered())
    {   
        std::string msg = "451 " + client->getNickname() + " :You have not registered\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (params.size() < 2)
    {
        std::string msg = "461 " + client->getNickname() + " MODE :Not enough parameters\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    std::map<std::string, Channel*> &channels = server->getChannels();
    std::string channelName = params[0];
    if (channels.find(channelName) == channels.end())
    {
        std::string msg = "403 " + client->getNickname() + " " + channelName + " :No such channel\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    Channel *channel = channels[channelName];
    std::string flag = params[1];
    if (!channel->ismember(client))
    {
        std::string msg = "482 " + client->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (!channel->isoperator(client))
    {
        std::string msg = "482 " + client->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return; 
    }
    if (flag.size() < 2 || (flag[0] != '-' && flag[0] != '+'))
    {
        return;
    }
    char sign = flag[0];
    char mode = flag[1];
    bool change = false;
    std::string modeParam = "";

    if (mode == 'i')
    {
        if (sign == '+' && !channel->isinviteonly())
        {
            channel->setinviteonly(true);
            change = true;
        }
        else if (sign == '-' && channel->isinviteonly())
        {
            channel->setinviteonly(false);
            change = true;
        }
    }
    else if (mode == 't')
    {
        if (sign == '+' && !channel->istopicrestricted())
        {
            channel->settopicrestricted(true);
            change = true;
        }
        else if (sign == '-' && channel->istopicrestricted())
        {
            channel->settopicrestricted(false);
            change = true;
        }
    }
    else if (mode == 'k')
    {
        if (sign == '+')
        {
            if (params.size() < 3)
            {
                std::string msg = "461 " + client->getNickname() + " MODE +k :Not enough parameters\r\n";
                send(client->getFd(), msg.c_str(), msg.size(), 0);
                return;
            }
            channel->setpassword(params[2]);
            modeParam = " " + params[2];
            change = true;
        }
        else if (sign == '-')
        {
            channel->setpassword("");
            change = true;
        }
    }
    else if (mode == 'o')
    {
        if (params.size() < 3)
        {
            std::string msg = "461 " + client->getNickname() + " MODE " + sign + "o :Not enough parameters\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }
        std::string targetNick = params[2];
        Client *targetClient = server->getClientNickName(targetNick);
        if (!targetClient)
        {
            std::string msg = "401 " + client->getNickname() + " " + targetNick + " :No such nick\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }
        if (!channel->ismember(targetClient))
        {
            std::string msg = "441 " + client->getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
            send(client->getFd(), msg.c_str(), msg.size(), 0);
            return;
        }
        if (sign == '+')
        {
            if (!channel->isoperator(targetClient))
            {
                channel->addoperator(targetClient);
                modeParam = " " + targetNick;
                change = true;
            }
        }
        else if (sign == '-')
        {
            if (channel->isoperator(targetClient))
            {
                channel->removeoperator(targetClient);
                modeParam = " " + targetNick;
                change = true;
            }
        }
    }
    else if (mode == 'l')
    {
        if (sign == '+')
        {
            if (params.size() < 3)
            {
                std::string msg = "461 " + client->getNickname() + " MODE +l :Not enough parameters\r\n";
                send(client->getFd(), msg.c_str(), msg.size(), 0);
                return;
            }
            std::string limitstr = params[2];
            int limit = atoi(limitstr.c_str());
            if (limit <= 0)
                return ;
            if (channel->getuserlimit() != limit)
            {
                channel->setuserlimit(limit);
                modeParam = " " + limitstr;
                change = true;
            }
        }
        else if (sign == '-')
        {
            if (channel->getuserlimit() > 0)
            {
                channel->setuserlimit(0);
                change = true;
            }
        }
    }
    else 
    {
        std::string msg = "472 " + client->getNickname() + " " + mode + " :is unknown mode char\r\n";
        send(client->getFd(), msg.c_str(), msg.size(), 0);
        return;
    }
    if (change)
    {
        std::string reply1 = ":" + client->getPrefix() + " MODE " + channelName + " " + sign + mode + modeParam;
        std::string reply2 = reply1 + "\r\n";
        send(client->getFd(), reply2.c_str(), reply2.size(), 0);
        channel->broadcast(reply1, client);
    }
};

void 	Server::handleChannelCommand(Client& client, const ParsedMessage& cmd)
{
    if (cmd.command == "TOPIC")
        handleTopic(&client , cmd.params , cmd.trailing, this);
    else if (cmd.command == "JOIN")
        handleJoin(&client , cmd.params, this);
    else if (cmd.command == "PART")
        handlePart(&client, cmd.params, cmd.trailing, this);
    else if (cmd.command == "INVITE")
        handleInvite(&client, cmd.params, this);
    else if (cmd.command == "KICK")
        handleKick(&client, cmd.params, cmd.trailing, this);
    else if (cmd.command == "MODE") 
        handleMode(&client, cmd.params, this);
};
