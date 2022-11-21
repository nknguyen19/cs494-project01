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
    vector<Question*> questions;
    vector<Player*> players;
    int currentQuestionIndex;
    int state;
    int currentPlayingPlayerIndex;
    int numberOfInGamePlayers;
    // int previousResult;

public:
    Game();
    ~Game();

    /*
    * Randomly select a set of questions from the question bank
    * Number of questions is at least 3 times the number of players
    */
    void initQuestions();


    /*
    * Check if the nickname is already taken in this game
    */
    bool isNicknameExist(string nickname);


    /*
    * Add a player to the game
    * Return true if the player is added successfully
    * Return false if the game is full
    * If the game is full, the state of the game will be changed to PLAYING 
    */
    bool addPlayer(Player* player);

    
    /*
    * Start the game
    */
    void startGame();


    /*
    * Get the full game status for the client to display
    * Format:
    *   <game state>
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
    * send mesaage to all players in the game
    */
    void notifyAllPlayers();


    /*
    * Return true if the game is started
    */
    bool isPlaying();

    /*
    * Return the current number of players in this game
    */
    int getCurrentNumberOfPlayers();

    /*
    * Return the current status of a player
    */
    int getPlayerStatus(int player_id);

    /**
    * Return true if the answer is correct.
    * Note that the whole game state after checking answer is also changed.
    */
    bool submitAnswer(char answer);


    static int WAITING;
    static int PLAYING;
    static int FINISHED;
};

#endif // GAME_SERVER_H