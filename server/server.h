#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <vector>
#include <map>
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

    vector<Game*> games;

    map<int, pair<int, int>> player_id;
    // mapping client socket id to the correspond pair (game id, in game player id)

public:
    Server();
    ~Server();

    /*
    * @brief: start the server
    */
    void run();

    /*
    * @brief: execute the message from the client
    * @param: message: the message from the client,
    *         client_socket: the socket of the client
    */
    void executeCommand(string message, int client_socket);

    /*
    * @brief: add a new player to the game
    * @param: client_socket: the socket of the player, 
    *        nickname: the nickname registered by the player
    * send the result message back to the client
    */
    void handleRegisterRequest(int client_socket, string nickname);

    /**
    * @brief: check if the player is in the game and in their turn
    */
    void handleInGameRequest(int client_socket);

    /**
    * @brief: handle answer request from client
    */
    void handleAnswerRequest(int client_socket, string answer);

    /**
    * @brief: handle move request from client
    */
    void handleMoveRequest(int client_socket);
};

#endif