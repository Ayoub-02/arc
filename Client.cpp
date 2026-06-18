#include "Client.hpp"

Client::Client(int socket)
{
    this->socket = socket;
    nickname = "";
    username = "";
    hostname = "";
    realname = "";
    isauthenticated = false;
    isregistered = false;
    buffer = "";
};

Client::~Client()
{

};

int Client::getsocket()
{
    return (socket);
};

std::string Client::getusername()
{
    return (username);
};

std::string Client::getnickname()
{
    return (nickname);
};

std::string Client::gethostname()
{
    return (hostname);
};

std::string Client::getrealname()
{
    return (realname);
};


bool  Client::getisauthenticated()
{
    return (isauthenticated);
};

bool Client::getisregistered()
{
    return (isregistered);
};

std::string Client::getbuffer()
{
    return (buffer);
};

void Client::setsocket(int socket)
{
    this->socket = socket;
};


void Client::setusername(std::string username)
{
    this->username = username;
};

void    Client::setnickname(std::string nickname)
{
    this->nickname = nickname;
};

void     Client::sethostname(std::string hostname)
{
    this->hostname = hostname;
};


void     Client::setrealname(std::string realname)
{
    this->realname = realname;
};


void Client::setisauthenticated(bool value)
{
    isauthenticated = value;
};


void    Client::setisregistered(bool value)
{
    isregistered = value;
};

std::string Client::getbuffer(std::string buffer)
{
    this->buffer = buffer;
};