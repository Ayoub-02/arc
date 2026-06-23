#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	Server server(atoi(argv[1]), argv[2]);
	
	try
	{
		signal(SIGINT, Server::handleSignal);
    	signal(SIGTERM, Server::handleSignal);
    	signal(SIGPIPE, SIG_IGN);


		std::cout << "Booting up the server ..." << std::endl;
		server.initSocket();

		std::cout << "Init complete, multiplexing time ...." << std::endl;
	
		server.startServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Server Fatal Error: " << e.what() << std::endl;
		server.cleanup();
        return 1;
	}

	std::cout << "\nSignal received. Shutting down gracefully..." << std::endl;
    server.cleanup();
	std::cout << "The Server Closed!" << std::endl;
}
