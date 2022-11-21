#ifndef PLAYER_H
#define PLAYER_H

#include <string>

using namespace std;

class Player
{
private:
    int socket;
    string nickname;
    int status;
    int canMoveTurn;

public:
    Player(int socket, string name);
    ~Player();

    int getSocket();
    string getNickname();
    int getStatus();
    void setStatus(int status);
    int getCanMoveTurn();
    void decreaseCanMoveTurn();

    static int INTURN;
    static int WAITING;
    static int WON;
    static int DISQUALIFIED;
};

#endif