#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	int port = std::atoi(argv[1]);
	Server server(port, argv[2]);
	
	std::cout << "Booting up the server ..." << std::endl;
	server.initSocket();
	server.startServer();

	std::cout << "inti complete, multiplexing time ....";

	

}