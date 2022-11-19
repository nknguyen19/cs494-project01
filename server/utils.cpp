#include "utils.h"
#include <iostream>
#include <time.h>

using namespace std;

bool validateNickname(string nickname)
{
    if (nickname.length() > 10)
    {
        return false;
    }
    for (int i = 0; i < nickname.length(); i++)
    {
        if (!isalnum(nickname[i]) && nickname[i] != '_')
        {
            return false;
        }
    }
    return true;
}

string parseMessageCommand(string message)
{
	string command = "";
	int i = 0;
	while (message[i] != ' ' && message[i] != '\0' && message[i] != '\r')
	{
		command += message[i];
		i++;
	}
	return command;
}

string parseMessageContent(string message)
{
	string content = "";
	int i = 0;
	while (i < message.length() && message[i] != ' ' && message[i] != '\0' && message[i] != '\r')
	{
		i++;
	}
	i++;
	while (i < message.length() && message[i] != '\0' && message[i] != '\r')
	{
		content += message[i];
		i++;
	}
	return content;
}

int random(int min, int max)
{
	srand(time(NULL));
	return rand() % (max - min + 1) + min;
}
