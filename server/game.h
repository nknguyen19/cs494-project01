#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "question.h"
#include <vector>

using namespace std;

class Player;

class Game {
private:
    int status;

    int maxNPlayers; // max number of players
    int nInGame; // number of in-game players
    int curPlayerIndex; // current playing player index
    vector<Player*> players;
    
    int curQuestionIndex; // current question index
    vector<Question*> questions;

public:
    static const int WAITING = 0;
    static const int PLAYING = 1;
    static const int FINISHED = 2;

    Game();
    Game(Player* player);
    ~Game();

    int getInQueueNumber();

    /*
    * Randomly select a set of questions from the question bank
    * Number of questions is at least 3 times the number of players
    */
    void initQuestions();

    /*
    * Add a player to the game
    * Return true if the player is added successfully
    * Return false if the game is full
    * If the game is full, the status of the game will be changed to PLAYING 
    */
    bool addPlayer(Player* player);

    /*
    * Start the game
    */
    void start();

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
    string getGameStatus();

    /*
    * send message to all players in the game
    */
    void notifyAllPlayers();


    /*
    * Return true if the game is started
    */
    bool isPlaying();
    bool isFinished();

    /*
    * Return the current number of players in this game
    */
    int getInGameNumber();

    void nextPlayer();

    /**
    * Return true if the answer is correct.
    * Note that the whole game status after checking answer is also changed.
    */
    bool submitAnswer(char answer);

    void remove(Player* player);

    void setStatus(int status);

    Player* currentPlayer();

    void moveTurn();

    vector<Player*> getInQueuePlayers();
};

#endif // GAME_SERVER_H