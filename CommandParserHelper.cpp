#include "CommandParser.hpp"

bool isValidParam(const std::string& param)
{
    if (param.empty())// not empty
        return false;

    if (param.size() > 9)//max 9 letters
        return false;

    for (size_t i = 0; i < param.size(); i++) // doesnt containt special char
	{
		if(!std::isalnum(static_cast<unsigned char>(param[i])) &&
			param[i] != '-' && param[i] != '_' &&
			param[i] != '[' && param[i] != ']' &&
			param[i] != '{' && param[i] != '}' &&
			param[i] != '\\' && param[i] != '|')
			{
				return false;
			}
	}
    return true;
}

void sendToClient(int fd, const std::string& message)
{
    send(fd, message.c_str(), message.size(), 0);
}

bool onlySpaces(std::string trailingMessage)
{
    bool onlySpaces = true;
    for (size_t i = 0; i < trailingMessage.size(); i++)
    {
        if (!isspace(static_cast<unsigned char>(trailingMessage[i])))
        {
            onlySpaces = false;
            break;
        }
    }
    return (onlySpaces);
}

