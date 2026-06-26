#include "Client.hpp"

Client::Client(int fd) : fd(fd)
{
    nickName = "";
    userName = "";
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
    return (userName);
};

std::string Client::getNickname()
{
    return (nickName);
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
    this->userName = username;
};

void    Client::setNickname(const std::string& nickname)
{
    this->nickName = nickname;
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

// Remove first n characters from buffer (after processing a command)
void Client::eraseBuffer(size_t n)
{
    if (n >= buffer.size())
        buffer.clear();
    else
        buffer.erase(0, n);
}

std::string Client::getPrefix() const
{
    return nickName + "!" + userName + "@" + hostname;
}
