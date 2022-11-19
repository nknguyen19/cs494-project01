#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "question.h"
#include <vector>
#include "player.h"

using namespace std;

class Game
{
private:
    int currentNumberOfPlayers;
    int maxNumberOfPlayers;
    vector<Question> questions;
    vector<Player> players;
    int currentQuestionIndex;
    int state;

public:
    Game();
    ~Game();

    // void run();
    // void initQuestions();
    bool isNicknameExist(string nickname);
    bool addPlayer(Player player);

    static int WAITING;
    static int PLAYING;
    static int FINISHED;
};

#endif // GAME_SERVER_H