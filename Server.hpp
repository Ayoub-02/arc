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
#include "Client.hpp"

class Server
{
private:
	int port;
	int serverFd;
	std::vector<pollfd> fds;
	std::vector<Client*> clients;

	void acceptNewClient();
	void handleClient(int index);

public:
	Server(int _port);
	~Server();

	void initSocket();
	void startServer();
};