#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>
#include <string.h>
#include <string>

#define PORT 54000
#define IP "127.0.0.1"

using namespace std;

class Client
{
private:
    int sock;
    sockaddr_in hint;
    char buffer[4096];
    string userInput;

public:
    Client();
    ~Client();

    void run();
};

#endif