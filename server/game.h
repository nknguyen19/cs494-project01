#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "question.h"
#include <vector>

#define WAITING 0
#define PLAYING 1
#define FINISHED 2

using namespace std;

class Game
{
private:
    int currentNumberOfPlayers;
    int maxNumberOfPlayers;
    vector<Question> questions;
    int currentQuestionIndex;
    int state;

public:
    Game();
    ~Game();

    void run();
    void initQuestions();
    void parseMessage(string message);
};

#endif // GAME_SERVER_H