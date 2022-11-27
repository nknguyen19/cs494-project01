#include "game.h"
#include "player.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/socket.h>

using namespace std;

#define DEFAULT_MAX_PLAYERS 3

Game::Game() : status(WAITING), max_players(DEFAULT_MAX_PLAYERS) { }

Game::Game(Player* player) :
    status(WAITING), max_players(DEFAULT_MAX_PLAYERS) {
    this->players.push_back(player);
}

Game::~Game() {
    for (Player* p: players) delete p;
    for (Question* q: questions) delete q;
}

string Game::getStatusMessage() {
    string message = "";
    message += to_string(this->status) + '\n';
    message += to_string(this->n_playing) + '\n';
    message += to_string(this->players.size()) + '\n';
    for (auto player: players)
        message += player->getNickname() + '\n'
            + to_string(player->getStatus()) + '\n'
            + to_string(player->getMoveTurn()) + '\n';
    if (this->status == PLAYING)
        message += this->questions[this->cur_question_id]->getQuestion() + '\n';
    else if (this->status == FINISHED)
        message += "Game finished. " + players[cur_player_id]->getNickname() + " is the winner!\n";
    return message;
}

bool Game::addPlayer(Player* player) {
    if (this->players.size() < this->max_players) {
        this->players.push_back(player);
        if (this->players.size() == this->max_players)
            this->start();
        return true;
    }
    return false;
}

void Game::initQuestions() {
    ifstream file("questions.txt");
    string line;
    int numberOfQuestions = random(max_players * 3, max_players * 5);
    
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
        // seek to the beginn_playing of the file
        file.clear();
        file.seekg(0, ios::beg);
        getline(file, line);
        Question* questionObj = new Question(question, choices, answer);
        questions.push_back(questionObj);
    }

    file.close();
}

void Game::start() {
    status = PLAYING;
    n_playing = players.size();
    cur_player_id = 0;
    players[cur_player_id]->setStatus(Player::INTURN);
    cur_question_id = 0;
    initQuestions();
}

void Game::notifyAllPlayers() {
    string message = getStatusMessage();
    for (auto player: players) {
        send(player->getSocketId(), message.c_str(), message.length(), 0);
    }
}

bool Game::isWaiting() { return status == WAITING; }
bool Game::isPlaying() { return status == PLAYING; }
bool Game::isFinished() { return status == FINISHED; }

void Game::nextPlayer() {
    int next_id;
    for (next_id = (cur_player_id + 1) % players.size();
        players[next_id]->isDisqualified();
        next_id = (next_id + 1) % players.size()
    );
    players[cur_player_id = next_id]->setStatus(Player::INTURN);
}

bool Game::submitAnswer(char answer) {
    if (questions[cur_question_id]->isCorrect(answer)) {
        ++cur_question_id;
        if (cur_question_id == questions.size() - 1){
            status = FINISHED;
            players[cur_player_id]->setStatus(Player::WON);
        }
        return true;
    }
    
    --n_playing;
    players[cur_player_id]->setStatus(Player::DISQUALIFIED);
    
    if (n_playing == 1) {
        status = FINISHED;
        for (auto player: players)
            if (!player->isDisqualified()) {
                player->setStatus(Player::WON);
                break;
            }
        return false;
    }

    nextPlayer();
    return false;
}

void Game::moveTurn() {
    players[cur_player_id]->useMoveTurn();
    players[cur_player_id]->setStatus(Player::WAITING);
    nextPlayer();
}

void Game::remove(Player* player) {
    if (isWaiting() || isPlaying()) {
        bool moved_turn = false;
        if (isPlaying() && player->isInTurn()) {
            moved_turn = true; this->nextPlayer();
        }

		auto ptr = find(players.begin(), players.end(), player);
		if (ptr == players.end())
			throw("500 Somehow this player does not exists in the game as we recorded.\n");

        delete player;
		players.erase(ptr);
        --n_playing;
        
        if (isPlaying()) {
            if (moved_turn && (ptr - players.begin()) < this->cur_player_id)
                --cur_player_id;
            if (n_playing == 1) status = FINISHED;
            notifyAllPlayers();
        }
    }
    else if (isFinished())
        throw("500 This player should already be removed when the game finished.\n");
    else
        throw("500 Unknown game status.\n");
}

vector<Player*> Game::getInGamePlayers() { return players; }