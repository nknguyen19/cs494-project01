#include "game.h"
#include <iostream>
#include <fstream>
#include "utils.h"
#include <algorithm>
#include <sys/socket.h>

using namespace std;

int Game::WAITING = 0;

int Game::PLAYING = 1;

int Game::FINISHED = 2;

Game::Game()
{
    this->currentNumberOfPlayers = 0;
    this->maxNumberOfPlayers = 2;
    this->state = WAITING;
}

Game::~Game()
{
}

bool Game::isNicknameExist(string nickname)
{
    for (auto player: players)
    {
        if (player->getNickname() == nickname)
        {
            return true;
        }
    }
    return false;
}

bool Game::addPlayer(Player* player)
{
    if (this->currentNumberOfPlayers < this->maxNumberOfPlayers)
    {
        this->players.push_back(player);
        this->currentNumberOfPlayers++;

        if (this->currentNumberOfPlayers == this->maxNumberOfPlayers)
        {
            // Game is full, start playing
            this->startGame();
        }

        return true;
    }

    return false;
}

void Game::startGame()
{
    initQuestions();
    this->state = PLAYING;
    this->currentPlayingPlayerIndex = 0;
    this->currentQuestionIndex = 0;
    this->numberOfInGamePlayers = this->currentNumberOfPlayers;
    players[this->currentPlayingPlayerIndex]->setStatus(Player::INTURN);
}

void Game::initQuestions() {
    ifstream file("questions.txt");
    string line;
    int numberOfQuestions = random(maxNumberOfPlayers * 3, maxNumberOfPlayers * 5);
    
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
        // seek to the beginning of the file
        file.clear();
        file.seekg(0, ios::beg);
        getline(file, line);
        Question* questionObj = new Question(question, choices, answer);
        questions.push_back(questionObj);
    }

    file.close();
}

string Game::getGameStatus()
{
    string gameStatus = "";
    gameStatus += to_string(this->state) + "\n";
    gameStatus += to_string(this->currentNumberOfPlayers) + "\n";
    gameStatus += to_string(this->maxNumberOfPlayers) + "\n";
    for (auto player: players)
    {
        gameStatus += player->getNickname() + 
                        "\n" + to_string(player->getStatus()) + 
                        "\n" + to_string(player->getCanMoveTurn()) +
                        "\n";
    }
    if (this->state == PLAYING)
    {
        gameStatus += this->questions[this->currentQuestionIndex]->getQuestion() + "\n";
    }
    else if (this->state == FINISHED)
    {
        gameStatus += "Game finished. " + players[currentPlayingPlayerIndex]->getNickname() + " is the winner!\n";
    }
    return gameStatus;
}

string Game::getPlayerNickname(int player_id) {
    return players[player_id]->getNickname();
}

void Game::notifyAllPlayers() {
    string gameStatus = this->getGameStatus();
    for (auto player: players) {
        send(player->getSocket(), gameStatus.c_str(), gameStatus.length(), 0);
    }
}

bool Game::isPlaying() {
    return this->state != WAITING;
}

int Game::getCurrentNumberOfPlayers() {
    return this->currentNumberOfPlayers;
}

int Game::getPlayerStatus(int player_id) {
    if (player_id < 0 || currentNumberOfPlayers <= player_id)
        return -1;
    else return this->players[player_id]->getStatus();
}

void Game::nextPlayer() {
    players[currentPlayingPlayerIndex]->setStatus(Player::WAITING);
    int next_id;
    for (next_id = (currentPlayingPlayerIndex + 1) % currentNumberOfPlayers;
        players[next_id]->getStatus() == Player::DISQUALIFIED;
        next_id = (next_id + 1) % currentNumberOfPlayers
    );
    players[currentPlayingPlayerIndex = next_id]->setStatus(Player::INTURN);
}

bool Game::submitAnswer(char answer) {
    if (questions[currentQuestionIndex]->isCorrect(answer)) {
        ++currentQuestionIndex;
        if (currentQuestionIndex == questions.size() - 1)
            state = Game::FINISHED;
        return true;
    }
    
    --numberOfInGamePlayers;
    if (numberOfInGamePlayers == 1) state = Game::FINISHED;
    nextPlayer();
    return false;
}

bool Game::currentPlayerMoveTurn() {
    if (players[currentPlayingPlayerIndex]->getCanMoveTurn() > 0) {
        players[currentPlayingPlayerIndex]->decreaseCanMoveTurn();
        nextPlayer();
        return true;
    }
    return false;
}