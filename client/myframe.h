#ifndef MYFRAME_H
#define MYFRAME_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <string>
#include "game.h"

#define PORT 54000
#define OK "OK"
#define CORRECT "CORRECT"
#define INCORRECT "INCORRECT"

using namespace std;

class MyFrame : public wxFrame
{
private:
    wxSocketClient *client;
    vector<wxControl*> controls;
    Game *game;
    wxTimer *timer;
    wxStaticText *timerText;
    int time;

public:
    MyFrame();
    ~MyFrame();

    bool connectServer();
    void sendMessage(string message);
    bool receiveMesage(string &message);
    string pasreMessageStatus(string message);
    string parseMessageContent(string message);
    string parseGameStatus(string message);
    void clearFrame();

    void OnHello(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnRegister(wxCommandEvent &event);
    void OnStart(wxCommandEvent &event);
    void OnAnswer(wxCommandEvent &event);
    void OnMoveTurn(wxCommandEvent &event);
    void OnLogout(wxCommandEvent &event);
    void OnNewGame(wxCommandEvent &event);
    void OnTimer(wxTimerEvent &event);
    void handleCorrectAnswer();
    void handleWrongAnswer();

    void showGreetingsFrame();
    void showRegisterFrame();
    void showGameFrame();
    void displayPlayers();
    void displayQuestion();
    void displayeStatus();
    void displayTimer();
    DECLARE_EVENT_TABLE();
};

enum
{
    ID_Hello = 1,
    ID_RegisterButton = 2,
    ID_UsernameTextbox = 3,
    ID_StartButton = 4,
    ID_LogOut = 5,
    ID_NewGame = 6,
    ID_Eliminated = 7,
    ID_PlayerNickname = 10,
    ID_Question = 20,
    ID_Answer = 21,
    ID_MyStatus = 30,
    ID_Result = 31,
    ID_MoveButton = 32,
    ID_NumberOfPlayer = 40,
    ID_NumberOfQuestion = 41,
    ID_EliminatedIcon = 42,
    ID_Timer = 50,
    ID_TimerText = 51
};

#endif
