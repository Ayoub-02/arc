#pragma once 

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <poll.h>
#include <vector>
#include <map>
#include <sstream>
#include "Client.hpp"

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
	std::vector<pollfd> fds;
	std::map<int, Client*> clients;

	void acceptNewClient();
	void handleClient(int index);

public:
	Server(int _port, const std::string& _password);
	~Server();

	void initSocket();
	void startServer();
};