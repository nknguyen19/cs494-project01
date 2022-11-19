#include "game.h"
#include <iostream>
#include <fstream>
#include "utils.h"
#include <algorithm>

using namespace std;

int Game::WAITING = 0;

int Game::PLAYING = 1;

int Game::FINISHED = 2;

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


        if (this->currentNumberOfPlayers == this->maxNumberOfPlayers)
        {
            this->state = PLAYING;
            initQuestions();
        }

        return true;
    }

    return false;
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

        Question questionObj(question, choices, answer);
        questions.push_back(questionObj);
    }

    file.close();
}
