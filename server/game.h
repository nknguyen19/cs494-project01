#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "question.h"
#include <vector>

using namespace std;

class Player;

class Game {
private:
    int status;

    int max_players; // max number of players
    int n_playing; // number of in-game players
    int cur_player_id; // current playing player index
    vector<Player*> players;
    
    int cur_question_id; // current question index
    vector<Question*> questions;

    /*
    * Get the full game status for the client to display
    * Format:
    *   <game status>
    *   <number of players>
    *   <max number of players>
    *   <player 1 nickname> 
    *   <player 1 status>
    *   <player 1 can move turn>
    *   ...
    *   <player n nickname>
    *   <player n status>
    *   <player n can move turn>
    *   <current question>
    *   <option A>
    *   <option B>
    *   <option C>
    *   <option D>
    */
    string getStatusMessage();

public:
    static const int WAITING = 0;
    static const int PLAYING = 1;
    static const int FINISHED = 2;

    Game();
    Game(Player* player);
    ~Game();

    bool addPlayer(Player* player);
    void initQuestions();
    void start();
    void notifyAllPlayers();

    bool isWaiting();
    bool isPlaying();
    bool isFinished();

    void nextPlayer();
    bool submitAnswer(char answer);
    void moveTurn();
    void remove(Player* player);

    vector<Player*> getInGamePlayers();
};

#endif // GAME_SERVER_H