#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <vector>
#include "game.h"

#define TRUE 1
#define FALSE 0
#define PORT 54000

using namespace std;

class Server
{
private:
    int master_socket; // master socket
    int max_clients; // max number of clients
    vector<int> client_socket;
    char buffer[1025];
    sockaddr_in address;
    fd_set readfds;
    // vector<Game> games;
public:
    Server();
    ~Server();

    void run();
};

#endif