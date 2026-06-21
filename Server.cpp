#include "Server.hpp"

Server::Server(int _port, const std::string& _password) : port(_port), serverFd(-1), password(_password) {}
Server::~Server()
{
	if (serverFd != -1)
		close(serverFd);
}

void Server::initSocket()
{
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	// configure the network address for the server so it knows where and how to listen for incoming connections
	sockaddr_in addr;

	addr.sin_family = AF_INET; //sets the address type to IPv4
	addr.sin_addr.s_addr = INADDR_ANY; //listen to every network connection available on the computer.
	addr.sin_port = htons(port);

	if (bind(serverFd, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(serverFd, 10) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	
	// adding server fd to poll
	pollfd pfd;
	pfd.fd = serverFd;
	pfd.events = POLL_IN;
	pfd.revents = 0;

	fds.push_back(pfd);

	std::cout << "Server linstening on port " << port << std::endl;
}

void Server::startServer()
{
	while (true)
	{
		int ret = poll(&fds[0], fds.size(), -1);
		if (ret < 0)
		{
			perror("poll");
			break;
		}

		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == serverFd)
					acceptNewClient();
				else
				{
					handleClient(i);
					i++;
				}
			}
		}
	}
}

void Server::acceptNewClient()
{
	sockaddr_in clientAddr;
	socklen_t len = sizeof(clientAddr);

	int clientFd = accept(serverFd, (sockaddr*)&clientAddr, &len);
	if (clientFd < 0)
	{
		perror("accept");
		return;
	}

	pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	fds.push_back(pfd);

	clients[clientFd] = new Client(clientFd);

	std::cout << "New client connected! FD = " << clientFd << std::endl;
}

void Server::handleClient(int index)
{
	int fd = fds[index].fd;
	Client* client = clients[fd];

	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));

	int bytes = recv(fds[index].fd, buffer, sizeof(buffer), 0);

	if (bytes <= 0)
	{
		std::cout << "Client disconnected FD = " << fds[index].fd << std::endl;
		close(fd);
		delete clients[fd];
		clients.erase(fd);
		fds.erase(fds.begin() + index);
		return;
	}

	client->appendBuffer(std::string(buffer, bytes));

	std::string &buf = client->getBufferRef();

	size_t pos;
	while ((pos = buf.find("\r\n")) != std::string::npos)
	{
		std::string message = buf.substr(0, pos);
		buf.erase(0, pos + 2);

		std::cout << "Full message: [" << message << "]" << std::endl;

		// parse mehdi belkass
		ParsedMessage parsed = parseMessage(message);

		//Debug (remove after)
		std::cout << "CMD: " << parsed.command << std::endl;

		//Exexute

	}
}


std::map<std::string, Channel*>& Server::getChannels()
{
	return (channels);
};


void	Server::removeMemberFromAllChannels(Client &client)
{
	std::map<std::string, Channel*>::iterator it = channels.begin();
	while (it != channels.end())
	{
		if (it->second->ismember(&client))
		{
			it->second->removememeber(&client);
			if (it->second->getmembers().empty())
            {
                delete it->second;
                std::map<std::string, Channel*>::iterator toErase = it;
                it++;
                channels.erase(toErase);
                continue;
            }
		}	
		it++;
	}
};



