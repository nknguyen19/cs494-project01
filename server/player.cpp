#include "player.h"
#include <iostream>

#define DEFAULT_MOVE_TURN 1

Player::Player(int socket_id, string nickname) :
    socket_id(socket_id), nickname(nickname),
    status(WAITING), move_turn(DEFAULT_MOVE_TURN) { }

Player::~Player() { }

int Player::getSocketId() { return socket_id; }
string Player::getNickname() { return nickname; }
int Player::getStatus() { return status; }
int Player::getMoveTurn() { return move_turn; }

string Player::getInfoMessage() {
    return getNickname() + '\n'
        + to_string(getStatus()) + '\n'
        + to_string(getMoveTurn()) + '\n';
}

bool Player::isInTurn() { return status == INTURN; }
bool Player::isWaiting() { return status == WAITING; }
bool Player::isDisqualified() { return status == DISQUALIFIED; }

void Player::setStatus(int status) { this->status = status; }

void Player::useMoveTurn() {
    if (this->move_turn <= 0)
		throw("400 You cannot move turn anymore.\n");
    --this->move_turn;
}