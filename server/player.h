#ifndef PLAYER_H
#define PLAYER_H

#include <string>

using namespace std;

class Game;

class Player {
private:
    int socket; // socket id
    Game* game;
    string nickname;
    int status; // in-game status
    int move_turn;

public:
    static const int INTURN = 0;
    static const int WAITING = 1;
    static const int WON = 2;
    static const int DISQUALIFIED = 3;

    Player(int socket_id, string nickname);
    ~Player();

    int getSocketId();
    Game* getGame();
    string getNickname();
    int getStatus();
    int getMoveTurn();

    void setGame(Game* game);
    void setStatus(int status);
    void useMoveTurn();

    void logOut();
};

#endif