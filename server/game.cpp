#include "game.h"
#include "player.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/socket.h>

using namespace std;

Game::Game() : status(WAITING), maxNPlayers(2) { }

Game::Game(Player* player) : status(WAITING), maxNPlayers(2) {
    this->players.push_back(player);
}

Game::~Game() {
    for (Question* q: questions) delete q;
}

int Game::getInQueueNumber() { return this->players.size(); }

bool Game::addPlayer(Player* player) {
    if (this->getInQueueNumber() < this->maxNPlayers) {
        this->players.push_back(player);
        if (this->getInQueueNumber() == this->maxNPlayers)
            this->start();
        return true;
    }
    return false;
}

void Game::start() {
    this->status = PLAYING;
    this->nInGame = this->getInQueueNumber();
    this->curPlayerIndex = 0;
    players[this->curPlayerIndex]->setStatus(Player::INTURN);
    this->curQuestionIndex = 0;
    this->initQuestions();
}

void Game::initQuestions() {
    ifstream file("questions.txt");
    string line;
    int numberOfQuestions = random(maxNPlayers * 3, maxNPlayers * 5);
    
    getline(file, line);

    int totalQuestions = stoi(line);
    
    vector<int> questionIndexes;
    for (int i = 0; i < totalQuestions; i++) {
        questionIndexes.push_back(i);
    }

    random_shuffle(questionIndexes.begin(), questionIndexes.end(), [](int i) { return random(0, i); });

    for (int i = 0; i < numberOfQuestions; i++) {
        int questionIndex = questionIndexes[i];
        string question;
        vector<string> choices;
        char answer;
        int lineIndex = 0;
        while (getline(file, line)) {
            if (lineIndex == questionIndex * 6) {
                question = line;
            } else if (lineIndex == questionIndex * 6 + 1) {
                choices.push_back(line);
            } else if (lineIndex == questionIndex * 6 + 2) {
                choices.push_back(line);
            } else if (lineIndex == questionIndex * 6 + 3) {
                choices.push_back(line);
            } else if (lineIndex == questionIndex * 6 + 4) {
                choices.push_back(line);
            } else if (lineIndex == questionIndex * 6 + 5) {
                answer = line[0];
            }
            lineIndex++;
        }
        // seek to the beginnInGame of the file
        file.clear();
        file.seekg(0, ios::beg);
        getline(file, line);
        Question* questionObj = new Question(question, choices, answer);
        questions.push_back(questionObj);
    }

    file.close();
}

string Game::getGameStatus() {
    string gameStatus = "";
    gameStatus += to_string(this->status) + "\n";
    gameStatus += to_string(this->nInGame) + "\n";
    gameStatus += to_string(this->maxNPlayers) + "\n";
    for (auto player: players)
        gameStatus += player->getNickname() + 
                        "\n" + to_string(player->getStatus()) + 
                        "\n" + to_string(player->getMoveTurn()) +
                        "\n";
    if (this->status == PLAYING)
        gameStatus += this->questions[this->curQuestionIndex]->getQuestion() + "\n";
    else if (this->status == FINISHED)
        gameStatus += "Game finished. " + players[curPlayerIndex]->getNickname() + " is the winner!\n";
    return gameStatus;
}

void Game::notifyAllPlayers() {
    string gameStatus = this->getGameStatus();
    for (auto player: players) {
        send(player->getSocketId(), gameStatus.c_str(), gameStatus.length(), 0);
    }
}

bool Game::isPlaying() { return this->status == PLAYING; }
bool Game::isFinished() { return this->status == FINISHED; }

int Game::getInGameNumber() { return this->nInGame; }

void Game::nextPlayer() {
    players[curPlayerIndex]->setStatus(Player::WAITING);
    int next_id;
    for (next_id = (curPlayerIndex + 1) % nInGame;
        players[next_id]->getStatus() == Player::DISQUALIFIED;
        next_id = (next_id + 1) % nInGame
    );
    players[curPlayerIndex = next_id]->setStatus(Player::INTURN);
}

bool Game::submitAnswer(char answer) {
    if (questions[curQuestionIndex]->isCorrect(answer)) {
        ++curQuestionIndex;
        if (curQuestionIndex == questions.size() - 1)
            this->setStatus(Game::FINISHED);
        return true;
    }
    
    --nInGame;
    if (nInGame == 1)
        this->setStatus(Game::FINISHED);
    nextPlayer();
    return false;
}

void Game::remove(Player* player) {
    if (this->status == Game::WAITING || this->status == Game::PLAYING) {
        bool moved_turn = false;
        if (this->status == Game::PLAYING && player->getStatus() == Player::INTURN) {
            moved_turn = true; this->nextPlayer();
        }

		int player_id;
		for (player_id = 0;
			players[player_id] != player && player_id < players.size();
			++player_id
		);
		if (player_id == players.size())
			throw("500 Somehow this player does not exists in the game as we recorded.\n");

		players.erase(players.begin() + player_id);
        --this->nInGame;
        
        if (this->status == Game::PLAYING) {
            if (moved_turn && player_id < this->curPlayerIndex)
                --this->curPlayerIndex;
            if (this->nInGame == 1)
                this->setStatus(Game::FINISHED);
            this->notifyAllPlayers();
        }
    }
    else if (this->status == Game::FINISHED)
        throw("500 This player should already be removed when the game finished.\n");
    else
        throw("500 Unknown game status.\n");
}

void Game::setStatus(int status) { this->status = status; }

Player* Game::currentPlayer() { return players[curPlayerIndex]; }

void Game::moveTurn() {
    players[curPlayerIndex]->useMoveTurn();
    this->nextPlayer();
}

vector<Player*> Game::getInQueuePlayers() { return players; }