#include "player.h"
#include "server.h"

Player::Player(int socket, string name)
{
    this->socket = socket;
    this->nickname = name;
    this->status = WAITING;
    this->canMoveTurn = 1;
}

Player::~Player()
{
    Server::removeSocketMapping(this->socket);
}

int Player::getSocket()
{
    return this->socket;
}

string Player::getNickname()
{
    return this->nickname;
}

int Player::getStatus()
{
    return this->status;
}

void Player::setStatus(int status)
{
    this->status = status;
}

int Player::getCanMoveTurn()
{
    return this->canMoveTurn;
}

void Player::decreaseCanMoveTurn()
{
    --this->canMoveTurn;
}

int Player::INTURN = 0;
int Player::WAITING = 1;
int Player::WON = 2;
int Player::DISQUALIFIED = 3;