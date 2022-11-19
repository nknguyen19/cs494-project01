#include "game.h"
#include <iostream>

using namespace std;

Game::Game()
{
    this->currentNumberOfPlayers = 0;
    this->maxNumberOfPlayers = 2;
    this->currentQuestionIndex = 0;
    this->state = WAITING;
}

Game::~Game()
{
}

bool Game::isNicknameExist(string nickname)
{
    for (auto player: players)
    {
        cout << player.getNickname() << endl;
        if (player.getNickname() == nickname)
        {
            return true;
        }
    }
    return false;
}

bool Game::addPlayer(Player player)
{
    if (this->currentNumberOfPlayers < this->maxNumberOfPlayers)
    {
        this->players.push_back(player);
        this->currentNumberOfPlayers++;
        return true;
    }
    return false;
}

int Game::WAITING = 0;
int Game::PLAYING = 1;
int Game::FINISHED = 2;
