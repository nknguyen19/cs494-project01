#ifndef MYFRAME_H
#define MYFRAME_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <string>

using namespace std;

class MyFrame : public wxFrame
{
private:
    wxSocketClient *client;

public:
    MyFrame(wxSocketClient *client);
    ~MyFrame();

    void sendMessage(string message);
    string receiveMesage();

    void OnHello(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnRegister(wxCommandEvent &event);
    DECLARE_EVENT_TABLE();
};

enum
{
    ID_Hello = 1,
    ID_RegisterButton = 2,
    ID_UsernameTextbox = 3
};

#endif
