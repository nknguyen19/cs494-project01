#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include "player.h"
#include "client.h"
#include "registerframe.h"
#include <wx/socket.h>

#define PORT 54000


class MyApp : public wxApp
{
private: 
    int state;
    Player *player;
    wxSocketClient *client;

public:
    virtual bool OnInit();
    void connectSerever();
    void sendMessage(string message);
    string receiveMessage();
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Hello, MyFrame::OnHello)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_BUTTON(ID_RegisterButton, MyFrame::OnRegister)
END_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);