#include "player.h"
#include "game.h"

#include <iostream>

Player::Player(int socket_id, string nickname) :
    socket(socket_id), game(nullptr), nickname(nickname), status(WAITING), move_turn(1) { }

Player::~Player() { }

int Player::getSocketId() { return this->socket; }

Game* Player::getGame() { return this->game; }

string Player::getNickname() { return this->nickname; }

int Player::getStatus() { return this->status; }

int Player::getMoveTurn() { return this->move_turn; }

void Player::setGame(Game* game) { this->game = game; }

void Player::setStatus(int status) { this->status = status; }

void Player::useMoveTurn() {
    if (this->move_turn <= 0)
		throw("400 You cannot move turn anymore.\n");
    --this->move_turn;
}

void Player::logOut() {
    if (this->game == nullptr) return;
    this->game->remove(this);
}