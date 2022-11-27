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

public:
    MyFrame();
    ~MyFrame();

    bool connectServer();
    void sendMessage(string message);
    string receiveMesage();
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
    void handleCorrectAnswer();
    void handleWrongAnswer();

    void showGreetingsFrame();
    void showRegisterFrame();
    void showGameFrame();
    DECLARE_EVENT_TABLE();
};

enum
{
    ID_Hello = 1,
    ID_RegisterButton = 2,
    ID_UsernameTextbox = 3,
    ID_StartButton = 4,
    ID_PlayerNickname = 10,
    ID_Question = 20,
    ID_Answer = 21,
    ID_MyStatus = 30,
    ID_Result = 31,
    ID_MoveButton = 32
};

#endif
