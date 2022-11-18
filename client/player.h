#ifndef PLAYER_H
#define PLAYER_H

#define WAITING 0
#define PLAYING 1
#define DISQUALIFIED 2

class Player
{
private:
    int token;
    int canMoveTurn;
    int status;

public:
    Player();
    ~Player();

    void anwser();
};

#endif