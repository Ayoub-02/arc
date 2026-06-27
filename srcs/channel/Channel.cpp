#include "../../includes/Channel.hpp"

Channel::Channel(std::string name)
{
    this->name = name;
    topic = "";
    password = "";
    user_limit = 0;
    invite_only = false;
    topic_restricted = false;

};

Channel::~Channel()
{
};

std::string Channel::getname()
{
   return (name);
};

std::string Channel::gettopic()
{
    return (topic);
};

std::string Channel::getpassword()
{
    return (password);
};

int Channel::getuserlimit()
{
    return (user_limit);
};

bool Channel::isinviteonly()
{
    return (invite_only);
};

bool Channel::istopicrestricted()
{
    return (topic_restricted);
};

std::vector<Client*> Channel::getmembers()
{
    return (members);
};

std::vector<Client*> Channel::getoperators()
{
    return (operators);
};

void    Channel::settopic(std::string topic)
{
    this->topic = topic;
};

void    Channel::setpassword(std::string password)
{
    this->password = password;
};

void    Channel::setuserlimit(int user_limit)
{
    this->user_limit = user_limit;
};

void    Channel::setinviteonly(bool invite_only)
{
    this->invite_only = invite_only;
};


void    Channel::settopicrestricted(bool topic_restricted)
{
    this->topic_restricted = topic_restricted;
};


void    Channel::addmember(Client *client)
{
    if (!ismember(client))
        members.push_back(client);
};

void    Channel::removememeber(Client *client)
{
    size_t i = 0;
    while (i < members.size())
    {
        if (members[i] == client)
        {
            members.erase(members.begin() + i);
            break;
        } 
        i++;
    }
    removeoperator(client);
};

bool Channel::ismember(Client *client)
{
    size_t i = 0;
    while (i < members.size())
    {
        if (members[i] == client)
            return (true);
        i++;
    }
    return (false);
};

void    Channel::addoperator(Client *client)
{
    if (ismember(client) && !isoperator(client))
        operators.push_back(client);  
};

void    Channel::removeoperator(Client *client)
{
    size_t i = 0;

    while (i < operators.size())
    {
        if (operators[i] == client)
        {
             operators.erase(operators.begin() + i);
             break;
        }
        i++;
    }
};

bool Channel::isoperator(Client *client)
{
    size_t i = 0;
    while (i < operators.size())
    {
        if (operators[i] == client)
            return (true);
        i++;
    }
    return (false);
};

void    Channel::broadcast(std::string msg, Client *sender)
{
    std::string fullmsg = msg + "\r\n";
    size_t i = 0;
    while (i < members.size())
    {
        if (members[i] != sender)
            send(members[i]->getFd(), fullmsg.c_str(),  fullmsg.size(), 0);
        i++;
    }
};


void    Channel::addInvite(Client *client)
{
    if (!isInvited(client))
    {
        invited.push_back(client);
    }
};

void        Channel::removeInvite(Client *client)
{
    std::vector<Client *>::iterator it = invited.begin();
    while (it != invited.end())
    {
        if (*it == client)
        {       
            invited.erase(it);
            return ;
        }  
        it++;
    }
};

bool    Channel::isInvited(Client *client)
{
    std::vector<Client *>::iterator it = invited.begin();
    while (it != invited.end())
    {
        if (*it == client)
            return (true);
        it++;
    }
    return (false);
};