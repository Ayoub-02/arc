#pragma once 

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <poll.h>
#include <vector>
#include <map>
#include <sstream>
#include <csignal>
#include "Client.hpp"
#include "Channel.hpp"
#include "UserCommands.hpp"
#include "ChannelCommands.hpp"


class Client;


enum CommandType {
    CMD_PASS, CMD_NICK,
	CMD_USER, CMD_QUIT,
    CMD_JOIN, CMD_PART, 
	CMD_TOPIC, CMD_INVITE,
	CMD_KICK, CMD_MODE,
    CMD_PRIVMSG,CMD_PING,
	CMD_UNKNOWN
};

struct ParsedMessage
{
    std::string command;
    std::vector<std::string> params;
    std::string trailing;
};

class Server
{
private:
	int port;
	int serverFd;
	std::string password;
	std::vector<pollfd> pollFds;
	std::map<int, Client*> clients;
	std::map<std::string, Channel*> channels;
	static bool signalFlag;

	void acceptNewClient();
	void handleClient(int index);

public:
	class NetworkException : public std::exception
	{
    	private:
        	const char* msg;
    	public:
        	NetworkException(const char* _msg);
        	virtual const char* what() const throw();
    };

	Server(int _port, const std::string& _password);
	~Server();

	void initSocket();
	void startServer();
 
	int getServerFd();
	std::map<int, Client*>& getClients();
	Client* getClient(int fd);
	std::string getServerPassword();

	static void handleSignal(int signum);
	ParsedMessage parseMessage(const std::string& rawMessage);
	CommandType getCommandType(const std::string& cmd);
	void routeCommand(int client_fd, const ParsedMessage& msg);
	void disconnectClient(int fd);
	void cleanup();
	bool	isNickTaken(std::string nickname);

	std::map<std::string, Channel*>& getChannels(); 
	void	handleClientCommand(Client& client, const ParsedMessage& cmd); 
	void 	handleChannelCommand(Client& client, const ParsedMessage& cmd, Server& server);
	void	removeMemberFromAllChannels(Client &client); 
	void	broadcastQuit(std::string reason, Client &client); 
	bool	clientExistence(std::string target); 
	bool	channelExistence(std::string target); 
	void	transferMessage(std::string target, const std::string  message, Client &client);
	Client *getClientNickName(std::string &nickName);

};
