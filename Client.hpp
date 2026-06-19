#pragma once

#include <iostream>
#include <sys/socket.h>

class Client
{
private:
    int fd;
    std::string nickname;
    std::string username;
    std::string hostname;
    std::string realname;
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
    bool getIsAuthenticated();
    bool getIsRegistered();
    std::string getBuffer();
    std::string& getBufferRef();

    // SETTERS
    void setFd(int fd);
    void setUsername(std::string& username);
    void setNickname(std::string& nickname);
    void setHostname(std::string& hostname);
    void setRealname(std::string& realname);
    void setIsAuthenticated(bool value);
    void setIsRegistered(bool value);

    // BUFFER
    void appendBuffer(std::string data);
    void eraseBuffer(size_t n);

    // UTILITY
    std::string getPrefix() const;
};