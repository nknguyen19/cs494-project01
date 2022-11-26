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
    map<int, Player*> players;
    // mapping client socket id to the correspond pair (game id, in game player id)

    void handleRegisterRequest(int socket_id, string nickname);
    void handleInGameRequest(int socket_id);
    void handleAnswerRequest(int socket_id, string answer);
    void handleMoveRequest(int socket_id);
    void handleLogoutRequest(int socket_id);

public:
    Server();
    ~Server();

    /**
    * @brief: start the server
    */
    void run();
    void processRequest(string request, int socket_id);

    void remove(Game* game);
    void remove(int client_socket);
};

#endif