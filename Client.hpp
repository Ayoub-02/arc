#pragma once

#include <iostream>

class Client
{
    private:
        int socket;
        std::string nickname;
        std::string username;
        std::string hostname;
        std::string realname;
        bool isauthenticated;
        bool isregistered;
        std::string buffer;
    public:
        Client(int socket);
        ~Client();

        //getters
        int getsocket();
        std::string getusername();
        std::string getnickname();
        std::string gethostname();
        std::string getrealname();
        bool getisauthenticated();
        bool getisregistered();
        std::string getbuffer();
        
        //setters
        void setsocket(int socket);
        void setusername(std::string username);
        void    setnickname(std::string nickname);
        void     sethostname(std::string hostname);
        void     setrealname(std::string realname);
        void setisauthenticated(bool value);
        void    setisregistered(bool value);
        std::string getbuffer(std::string buffer);
        
        //utility
        void    sendMessage(std::string message);
        void    appendBuffer(std::string data);
        void    clearBuffer();


        std::string getPrefix();

};