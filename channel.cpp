#include "channel.hpp"

Channel::Channel(std::string name)
{
    this->name = name;
    
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

Channel::