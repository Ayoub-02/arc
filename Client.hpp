#pragma once

#include <iostream>
#include <sys/socket.h>
#include "Server.hpp"

class Client
{
private:
    int fd;
    std::string nickname;
    std::string username;
    std::string hostname;
    std::string realname;
    std::string servername;  //added by mehdi 
    bool isAuthenticated;
    bool isRegistered;
    std::string buffer;

public:
    Client(int fd);
    ~Client();

    // GETTERS
    int getFd();
    std::string getUsername();
    std::string getNickname();
    std::string getHostname();
    std::string getRealname();
    std::string getServername();   //added by mehdi 
    bool getIsAuthenticated();
    bool getIsRegistered();
    std::string getBuffer();
    std::string& getBufferRef();

    // SETTERS
    void setFd(int fd);
    void setUsername(const std::string& username);
    void setNickname(const std::string& nickname);
    void setHostname(const std::string& hostname);
    void setRealname(const std::string& realname);
    void setServername(const std::string& realname);  //added by mehdi
    void setIsAuthenticated(bool value);
    void setIsRegistered(bool value);
    void setBuffer(std::string buffer)
    {
        this->buffer = buffer;
    }

    // BUFFER
    void appendBuffer(std::string data);
    void eraseBuffer(size_t n);

    // UTILITY
    std::string getPrefix() const;
};