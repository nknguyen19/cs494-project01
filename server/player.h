#ifndef PLAYER_H
#define PLAYER_H

#include <string>

using namespace std;

class Game;

class Player {
private:
    int socket_id;
    string nickname;
    int status;
    int move_turn;

public:
    static const int INTURN = 0;
    static const int WAITING = 1;
    static const int WON = 2;
    static const int DISQUALIFIED = 3;

    Player(int socket_id, string nickname);
    ~Player();

    int getSocketId();
    string getNickname();
    int getStatus();
    int getMoveTurn();
    string getInfoMessage();

    bool isInTurn();
    bool isWaiting();
    bool isDisqualified();

    void setStatus(int status);
    void useMoveTurn();
};

#endif