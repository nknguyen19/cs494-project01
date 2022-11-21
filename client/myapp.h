#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include "client.h"
#include "myframe.h"
#include <wx/socket.h>

#define PORT 54000


class MyApp : public wxApp
{
private: 
    int state;
    wxSocketClient *client;
// 
public:
    virtual bool OnInit();
    // void connectSerever();
    // void sendMessage(string message);
    // string receiveMessage();
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Hello, MyFrame::OnHello)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_BUTTON(ID_RegisterButton, MyFrame::OnRegister)
END_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);