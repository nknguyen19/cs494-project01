#ifndef MYFRAME_H
#define MYFRAME_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <string>
#include "game.h"

using namespace std;

class MyFrame : public wxFrame
{
private:
    wxSocketClient *client;
    vector<wxControl*> controls;
    Game *game;

public:
    MyFrame(wxSocketClient *client);
    ~MyFrame();

    void sendMessage(string message);
    string receiveMesage();

    void OnHello(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnRegister(wxCommandEvent &event);
    string pasreMessageStatus(string message);
    string parseMessageContent(string message);
    string parseGameStatus(string message);
    void clearFrame();
    void showRegisterFrame();
    void showGameFrame();
    DECLARE_EVENT_TABLE();
};

enum
{
    ID_Hello = 1,
    ID_RegisterButton = 2,
    ID_UsernameTextbox = 3,
    ID_PlayerNickname = 10,
    ID_Question = 20,
    ID_Answer = 21,
};

#endif
