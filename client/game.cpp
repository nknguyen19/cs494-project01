#include "game.h"
#include <sstream>
#include <iostream>

int Game::GAME_WAITING = 0;

int Game::GAME_PLAYING = 1;

int Game::GAME_FINISHED = 2;

int Game::PLAYER_INTURN = 0;

int Game::PLAYER_WAITING = 1;

int Game::PLAYER_WON = 2;

int Game::PLAYER_DISQUALIFIED = 3;

Game::Game(string myNickname)
{
    this->status = GAME_WAITING;
    this->isMyTurn = false;
    this->myNickname = myNickname;
    this->numberOfPlayers = 0;
    this->numberOfPlayingPlayers = 0;
}

Game::~Game()
{
}

void Game::update(string gameStatus)
{
    // get line by line from gameStatus and update game
    try {
        string line;
        istringstream f(gameStatus);
        int status, numberOfPlayers, numberOfPlayingPlayers;
        vector<Player*> players;
        Question *currentQuestion;

        // get game status
        getline(f, line);
        status = stoi(line);
        this->status = status;

        // get number of playing players
        getline(f, line);
        numberOfPlayingPlayers = stoi(line);
        this->numberOfPlayingPlayers = numberOfPlayingPlayers;

        // get number of players
        getline(f, line);
        numberOfPlayers = stoi(line);
        this->numberOfPlayers = numberOfPlayers;

        // get players
        for (int i = 0; i < numberOfPlayers; i++) {
            getline(f, line);
            string nickname = line;

            getline(f, line);
            int status = stoi(line);
            if (nickname == this->myNickname) {
                if (status == PLAYER_INTURN) {
                    this->isMyTurn = true;
                } else {
                    this->isMyTurn = false;
                }
            }

            getline(f, line);
            int canMoveTurn = stoi(line);
            players.push_back(new Player(nickname, canMoveTurn, status));

            if (status == PLAYER_WON) {
                this->winner = nickname;
            }
        }
        this->players = players;

        if (status == GAME_PLAYING) {
            // get question
            getline(f, line);
            string question = line;

            // get answers
            vector<string> answers;
            for (int i = 0; i < 4; i++) {
                getline(f, line);
                answers.push_back(line);
            }
            currentQuestion = new Question(question, answers);
            this->currentQuestion = currentQuestion;
        }
    } catch (exception e) {
        cout << "Error while parsing game status" << endl;
    }
    
}

vector<Player*> Game::getPlayers()
{
    return this->players;
}

int Game::getStatus()
{
    return this->status;
}

bool Game::getIsMyTurn()
{
    return this->isMyTurn;
}

int Game::getNumberOfPlayers()
{
    return this->numberOfPlayers;
}

int Game::getNumberOfPlayingPlayers()
{
    return this->numberOfPlayingPlayers;
}

Question* Game::getCurrentQuestion()
{
    return this->currentQuestion;
}

string Game::getMyNickname()
{
    return this->myNickname;
}

bool Game::isPlaying()
{
    return this->status == GAME_PLAYING;
}

bool Game::isWaiting()
{
    return this->status == GAME_WAITING;
}

bool Game::isFinished()
{
    return this->status == GAME_FINISHED;
}

string Game::getWinner()
{
    return this->winner;
}

Player Game::getCurrentPlayingPlayer()
{
    for (int i = 0; i < this->players.size(); i++) {
        if (this->players[i]->status == PLAYER_INTURN) {
            return *this->players[i];
        }
    }
    return *this->players[0];
}

Player Game::getMyPlayer()
{
    for (int i = 0; i < this->players.size(); i++) {
        if (this->players[i]->nickname == this->myNickname) {
            return *this->players[i];
        }
    }
    return *this->players[0];
}

string Game::getResult()
{
    return this->getWinner() == this->getMyNickname() ? "You won!" : ("You lost! The winner is " + this->getWinner());
}

bool Game::isEliminated()
{
    return this->getMyPlayer().status == PLAYER_DISQUALIFIED;
}
