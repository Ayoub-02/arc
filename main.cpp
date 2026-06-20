
#include "CommandParser.hpp"
#include "Client.hpp"

int main()
{
    Client client(2);
    // client.setBuffer("USER username 0  * :\r\n");

    // Command cmd;
    Server server;
    // parseCommand(client, cmd, server);
    

    Command cmd;
    cmd.command_name = "USER";
    cmd.params.push_back("dfv");
    cmd.params.push_back("0");
    cmd.params.push_back("*");
    cmd.params.push_back("hi");
    client.setIsAuthenticated(1);
    client.setIsRegistered(1);

    handleUser(client, cmd, server);
    // std::cout << "thats the command name: " << std::endl;
    // std::cout << cmd.command_name << std::endl;
    // std::cout << "and thats the arguments: " << std::endl;
    // for (size_t i = 0; i < cmd.params.size(); i++)
    //     std::cout << cmd.params[i] << std::endl;
}
  