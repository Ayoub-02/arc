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
	std::map<std::string, Channel*> channels;//added by redippo
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

	// getters 
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

	std::map<std::string, Channel*>& getChannels(); //roundi
	void	handleClientCommand(Client& client, const ParsedMessage& cmd); //ljadid dyal mehdi
	void 	handleChannelCommand(Client& client, const ParsedMessage& cmd, Server& server);
	void	removeMemberFromAllChannels(Client &client); //ljadid dyal mehdi
	void	broadcastQuit(std::string reason, Client &client); //ljadid dyal mehdi
	bool	clientExistence(std::string target); //ljadid dyal mehdi
	bool	channelExistence(std::string target); ////ljadid dyal mehdi
	void	transferMessage(std::string target, const std::string  message, Client &client);//ljadid dyal mehdi
	Client *getClientNickName(std::string &nickName); // ljadid dyal roundi

};
