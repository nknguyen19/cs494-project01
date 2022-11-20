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
    vector<Game*> games;
public:
    Server();
    ~Server();

    /*
    * @brief: start the server
    */
    void run();


    /*
    * @brief: add a new player to the game
    * @param: client_socket: the socket of the player, 
    *        content: the nickname registered by the player
    * send the result message back to the client
    */
    void registerPlayer(int client_socket, string content);


    /*
    * @brief: execute the message from the client
    * @param: message: the message from the client,
    *         client_socket: the socket of the client
    */
    void executeCommand(string message, int client_socket);
};

#endif