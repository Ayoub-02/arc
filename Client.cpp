#include "Client.hpp"

Client::Client(int fd) : fd(fd)
{
    nickname = "";
    username = "";
    hostname = "";
    realname = "";
    isAuthenticated = false;
    isRegistered = false;
    buffer = "";
};

Client::~Client()
{

};

int Client::getFd()
{
    return (fd);
};

std::string Client::getUsername()
{
    return (username);
};

std::string Client::getNickname()
{
    return (nickname);
};

std::string Client::getHostname()
{
    return (hostname);
};

std::string Client::getRealname()
{
    return (realname);
};

std::string Client::getServername()
{
    return (servername);
}

bool  Client::getIsAuthenticated()
{
    return (isAuthenticated);
};

bool Client::getIsRegistered()
{
    return (isRegistered);
};

std::string Client::getBuffer()
{
    return (buffer);
};
std::string& Client::getBufferRef()
{
    std::string &bufRef = buffer;
    return (bufRef);
};

void Client::setFd(int fd)
{
    this->fd = fd;
};

void Client::setUsername(const std::string& username)
{
    this->username = username;
};

void    Client::setNickname(const std::string& nickname)
{
    this->nickname = nickname;
};

void     Client::setHostname(const std::string& hostname)
{
    this->hostname = hostname;
};


void     Client::setRealname(const std::string& realname)
{
    this->realname = realname;
};

void Client::setServername(const std::string& servername)
{
    this->servername = servername;
}


void Client::setIsAuthenticated(bool value)
{
    isAuthenticated = value;
};


void    Client::setIsRegistered(bool value)
{
    isRegistered = value;
};
void Client::appendBuffer(std::string data)
{
    buffer += data;
}

void Client::eraseBuffer(size_t n)
{
    if (n >= buffer.size())
        buffer.clear();
    else
        buffer.erase(0, n);
}

std::string Client::getPrefix() const
{
    return nickname + "!" + username + "@" + hostname;
}
