#pragma once


#include <iostream>
#include <string>
#include <vector>

class Client;

class Channel
{
    private:
        std::string name;
        std::string topic;
        std::string password;
        int user_limit;
        bool    invite_only;
        bool    topic_restricted;
        std::vector<Client*> members;
        std::vector<Client*> operators;
    public:
        Channel(std::string name);
        ~Channel();

        //getters
        std::string getname();
        std::string gettopic();
        std::string getpassword();
        int getuserlimit();
        bool isinviteonly();
        bool istopicrestricted();
        std::vector<Client*> getmembers();
        std::vector<Client*> getoperators();
        //setters
        void    settopic(std::string topic);
        void    setpassword(std::string password);
        void    setuserlimit(int user_limit);
        void    setinviteonly(bool invite_only);
        void    settopicrestricted(bool topic_restricted);
        //member management
        void    addmember(Client *client);
        void    removememeber(Client *client);
        bool ismember(Client *client);
        //operator management
        void    addoperator(Client *client);
        void    removeoperator(Client *client);
        bool    isoperator(Client *client);

        //broadcast
        void    broadcast(std::string msg, Client *client);

        //added by mehdi
        void removeMember(Client* client);
        void removeOperator(Client* client);

        

};