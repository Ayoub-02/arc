#include "Server.hpp"

Server::NetworkException::NetworkException(const char* _msg) : msg(_msg) {}

const char* Server::NetworkException::what() const throw() {
    return msg;
}

Server::Server(int _port, const std::string& _password) : port(_port), serverFd(-1), password(_password) {}
Server::~Server()
{
	if (serverFd != -1)
		close(serverFd);
}

bool Server::signalFlag = false;
void Server::handleSignal(int signum)
{
	(void)signum;
	Server::signalFlag = true;
}

int Server::getServerFd() { return serverFd; }
std::string Server::getServerPassword() { return password; }
std::map<int, Client*>& Server::getClients() { return clients; }
Client* Server::getClient(int fd)
{
    if (clients.find(fd) != clients.end())
        return clients[fd];
	
    return NULL;
}

void Server::initSocket()
{
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd < 0)
		throw NetworkException("Failed to ceate socket.");

	int opt = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw NetworkException("Failed to set socket options.");
	
	if (fcntl(serverFd, F_SETFL, O_NONBLOCK) == -1)
		throw NetworkException("Fialed to set non-blocking mode.");
	
	// configure the network address for the server so it knows where and how to listen for incoming connections
	sockaddr_in addr;

	addr.sin_family = AF_INET; //sets the address type to IPv4
	addr.sin_addr.s_addr = INADDR_ANY; //listen to every network connection available on the computer.
	addr.sin_port = htons(port);

	if (bind(serverFd, (sockaddr*)&addr, sizeof(addr)) < 0)
		throw NetworkException("Failed to bind to port");

	if (listen(serverFd, 10) < 0)
		throw NetworkException("Failed to listen on socket.");
	
	// adding server fd to poll
	pollfd pfd;
	pfd.fd = serverFd;
	pfd.events = POLL_IN;
	pfd.revents = 0;

	pollFds.push_back(pfd);

	std::cout << "Server linstening on port " << port << std::endl;
}

void Server::startServer()
{
	while (!signalFlag)
	{
		int ret = poll(&pollFds[0], pollFds.size(), -1);
		if (ret < 0 && !signalFlag)
		{
            std::cerr << "Fatal Error: poll() failed." << std::endl;
            break;
        }
		else if (signalFlag)
            break;

		for (size_t i = 0; i < pollFds.size(); i++)
		{
			if (pollFds[i].revents & POLLIN)
			{
				if (pollFds[i].fd == serverFd)
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
		std::cerr << "Warning: Failed to accept a new connection." << std::endl;
		return;
	}

	pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	pollFds.push_back(pfd);

	clients[clientFd] = new Client(clientFd);

	std::cout << "New client connected! FD = " << clientFd << std::endl;
}

ParsedMessage Server::parseMessage(const std::string& rawMessage) {
    ParsedMessage msg;
    std::string str = rawMessage;

    size_t start = str.find_first_not_of(' ');
    if (start != std::string::npos)
		str = str.substr(start);

    size_t pos = str.find(' ');
    if (pos != std::string::npos)
	{
        msg.command = str.substr(0, pos);
        str.erase(0, pos);
    }
	else
        msg.command = str;

	for (size_t i = 0; i < msg.command.length(); ++i)
        msg.command[i] = toupper(msg.command[i]);

	if (pos == std::string::npos)
        return msg;
    while (!str.empty())
	{
        start = str.find_first_not_of(' ');
        if (start == std::string::npos)
			break;
        str = str.substr(start);

        if (str[0] == ':') 
		{
            msg.trailing = str.substr(1);
            break;
        }

        pos = str.find(' ');
        if (pos != std::string::npos)
		{
            msg.params.push_back(str.substr(0, pos));
            str.erase(0, pos);
        }
		else 
		{
            msg.params.push_back(str);
            break;
        }
    }
    return msg;
}

CommandType Server::getCommandType(const std::string& cmd)
{
    if (cmd == "PASS") return CMD_PASS;
    if (cmd == "NICK") return CMD_NICK;
    if (cmd == "USER") return CMD_USER;
    if (cmd == "QUIT") return CMD_QUIT;
    if (cmd == "JOIN") return CMD_JOIN;
    if (cmd == "PART") return CMD_PART;
    if (cmd == "TOPIC") return CMD_TOPIC;
    if (cmd == "INVITE") return CMD_INVITE;
    if (cmd == "KICK") return CMD_KICK;
    if (cmd == "MODE") return CMD_MODE;
    if (cmd == "PRIVMSG") return CMD_PRIVMSG;
    return CMD_UNKNOWN;
}

void Server::routeCommand(int client_fd, const ParsedMessage& msg)
{
	Client* current_client = clients[client_fd];
	CommandType type = getCommandType(msg.command);
    switch (type) {
        case CMD_PASS:
        case CMD_NICK:
        case CMD_USER:
        case CMD_QUIT:
        case CMD_PRIVMSG:
        {
            handleClientCommand(*current_client, msg, *this);
            break;
        }

        case CMD_JOIN:
        case CMD_PART:
        case CMD_TOPIC:
        case CMD_INVITE:
        case CMD_KICK:
        case CMD_MODE:
            // handleChannelCommand(current_client, msg, this);
            break;

        case CMD_UNKNOWN:
        default:
        {
            std::cout << "Network: Unhandled command '" << msg.command << "' received from FD " << client_fd << std::endl;
            sendToClient(client_fd, "421 ERR_UNKNOWNCOMMAND :Unknown command\r\n");
            break;
        }
    }	
}

void Server::handleClient(int index)
{
	int fd = pollFds[index].fd;
	Client* client = clients[fd];

	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));

	int bytes = recv(pollFds[index].fd, buffer, sizeof(buffer), 0);

	if (bytes <= 0)
	{
		std::cout << "Client disconnected FD = " << pollFds[index].fd << std::endl;
		disconnectClient(fd);
		return;
	}

	client->appendBuffer(std::string(buffer, bytes));

	std::string &buf = client->getBufferRef();

	size_t pos;
	while ((pos = buf.find("\r\n")) != std::string::npos)
	{
		std::string message = buf.substr(0, pos);
		buf.erase(0, pos + 2);

		// std::cout << "Full message: [" << message << "]" << std::endl;

		// parse
		ParsedMessage parsedMsg = parseMessage(message);
		

		//Debug (remove after)
		// std::cout << "CMD: " << parsedMsg.command << " Trailing: " << parsedMsg.trailing << std::endl;
		routeCommand(fd, parsedMsg);
		//Exexute

	}
}

bool Server::isNickTaken(std::string nickname)
{
    for (std::map<int, Client*>::iterator it = clients.begin();it != clients.end();++it)
    {
        if (it->second->getNickname() == nickname)
            return true;
    }
    return false;
}

void Server::disconnectClient(int fd)
{
	if (clients.find(fd) == clients.end())
		return;
	close(fd);
	delete clients[fd];
	clients.erase(fd);

    for (std::vector<struct pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it) 
	{
        if (it->fd == fd)
		{
            pollFds.erase(it);
            break;
        }
    }
}

void Server::cleanup()
{
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        close(it->first);
        delete it->second;
    }
    clients.clear(); // Wipe the map to remove dangling pointers

    // 2. Close the main server socket
    if (serverFd != -1) {
        close(serverFd);
        serverFd = -1; // Reset to prevent double-close
    }

    // Clear the poll array
    pollFds.clear();
    
    std::cout << "Network: All resources successfully cleaned up." << std::endl;
}

void    Server::removeMemberFromAllChannels(Client &client)
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
}

bool   Server::clientExistence(std::string target)
{
    if (target.empty() || target[0] == '#')
        return false;

    for (std::map<int, Client*>::iterator it = clients.begin();
            it != clients.end(); ++it)
    {
        Client* c = it->second;
        if (c->getNickname() == target)
            return true;
    }
    return false;
}

bool Server::channelExistence(std::string target)
{
    if (target.empty() || target[0] != '#')
        return false;

    return channels.find(target) != channels.end();
}

std::map<std::string, Channel*>& Server::getChannels()
{
	return (channels);
};

void	Server::transferMessage(std::string target, const std::string  message, Client &client)
{
    if (target[0] != '#')
    {
        for (std::map<int, Client*>::iterator it = clients.begin();
                it != clients.end(); ++it)
        {
            Client* c = it->second;
            if (c->getNickname() == target)
            {
                sendToClient(c->getFd(), message);
                return;
            }
        }
    }
    else
    {
        std::map<std::string, Channel*>& channels = getChannels();
        std::map<std::string, Channel*>::iterator it = channels.find(target);
        if (it != channels.end())
        {
            Channel* channel = it->second;
            channel->broadcast(message, &client);
        }
    }
}

Client *Server::getClientNickName(std::string &nickName)
{
	std::map<int, Client*>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if (it->second->getNickname() == nickName)
			return (it->second);
		it++;
	}
	return (NULL);
}

