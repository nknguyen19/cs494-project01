#include "myframe.h"

MyFrame::MyFrame(wxSocketClient *client) : wxFrame(nullptr, wxID_ANY, "Hello World")
{
    this->client = client;
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    // SetStatusText("Welcome to wxWidgets!");

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    wxButton *registerButton = new wxButton(this, ID_RegisterButton, "Register", wxPoint(200, 100), wxSize(100, 50));

    wxTextCtrl *username_textbox = new wxTextCtrl(this, ID_UsernameTextbox, "", wxPoint(100, 30), wxSize(300, 50));
    Bind(wxEVT_BUTTON, &MyFrame::OnRegister, this, ID_RegisterButton);
}

MyFrame::~MyFrame()
{
}

void MyFrame::sendMessage(string message)
{
    int res = (client->Write(message.c_str(), message.length())).LastCount();
}

string MyFrame::receiveMesage()
{
    char buffer[1024];
    int res = (client->Read(buffer, 1024)).LastCount();
    return string(buffer, res);
}

void MyFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent &event)
{
    wxLogMessage("Hello world from wxWidgets!");
}



void MyFrame::OnRegister(wxCommandEvent &event)
{
    wxTextCtrl *username_textbox = (wxTextCtrl *)FindWindow(ID_UsernameTextbox);
    wxString username = username_textbox->GetValue();
    this->sendMessage("REGISTER " + string(username));
    string response = this->receiveMesage();
    wxLogMessage("Response: %s", response);
}