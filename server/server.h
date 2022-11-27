#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <vector>
#include <map>
#include "game.h"
#include "player.h"

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

    vector<Game*> games;
    map<int, pair<Player*, Game*>> descriptor;
    // mapping client socket id to the correspond pair of pointers

    void handleRegisterCommand(int socket_id, string nickname);
    void handleInGameCommand(int socket_id);
    void handleAnswerCommand(int socket_id, string answer);
    void handleMoveCommand(int socket_id);
    void handleLogoutCommand(int socket_id);
    void handleInfoCommand(int socket_id);

    void remove(Game* game);
    void remove(int socket_id);

public:
    Server();
    ~Server();

    /**
    * @brief: start the server
    */
    void run();
    void executeCommand(string message, int socket_id);
};

#endif