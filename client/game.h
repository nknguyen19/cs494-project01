#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

using namespace std;

struct Player {
    string nickname;
    int canMoveTurn;
    int status;
    
    Player(string nickname, int canMoveTurn, int status) {
        this->nickname = nickname;
        this->canMoveTurn = canMoveTurn;
        this->status = status;
    }
};

struct Question {
    string question;
    vector<string> answers;
    
    Question(string question, vector<string> answers) {
        this->question = question;
        this->answers = answers;
    }
};

class Game
{
private:
    vector<Player*> players;
    int status;
    bool isMyTurn;
    int numberOfPlayers;
    int numberOfPlayingPlayers;
    Question *currentQuestion;
    string myNickname;
    string winner;

public:
    Game(string myNickname);
    ~Game();
    
    void update(string gameStatus);
    vector<Player*> getPlayers();
    int getStatus();
    bool getIsMyTurn();
    int getNumberOfPlayers();
    int getNumberOfPlayingPlayers();
    Question* getCurrentQuestion();
    string getMyNickname();
    bool isPlaying();
    bool isWaiting();
    bool isFinished();
    string getWinner();
    Player getCurrentPlayingPlayer();
    Player getMyPlayer();
    string getResult();
    bool isEliminated();

    static int GAME_WAITING;
    static int GAME_PLAYING;
    static int GAME_FINISHED;  
    static int PLAYER_INTURN;
    static int PLAYER_WAITING;
    static int PLAYER_WON;
    static int PLAYER_DISQUALIFIED;
};

#endif
