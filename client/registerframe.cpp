// #include "registerframe.h"
// RegisterFrame::RegisterFrame(wxSocketClient *client) : MyFrame(client)
//     // : wxFrame(nullptr, wxID_ANY, "Hello World")
// {
//     wxMenu *menuFile = new wxMenu;
//     menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
//                      "Help string shown in status bar for this menu item");
//     menuFile->AppendSeparator();
//     menuFile->Append(wxID_EXIT);

//     wxMenu *menuHelp = new wxMenu;
//     menuHelp->Append(wxID_ABOUT);

//     wxMenuBar *menuBar = new wxMenuBar;
//     menuBar->Append(menuFile, "&File");
//     menuBar->Append(menuHelp, "&Help");

//     SetMenuBar(menuBar);

//     CreateStatusBar();
//     // SetStatusText("Welcome to wxWidgets!");

//     Bind(wxEVT_MENU, &RegisterFrame::OnHello, this, ID_Hello);
//     Bind(wxEVT_MENU, &RegisterFrame::OnAbout, this, wxID_ABOUT);
//     Bind(wxEVT_MENU, &RegisterFrame::OnExit, this, wxID_EXIT);

//     wxButton *registerButton = new wxButton(this, ID_RegisterButton, "Register", wxPoint(200, 100), wxSize(100, 50));
//     registerButton->Destroy();

//     wxTextCtrl *username_textbox = new wxTextCtrl(this, ID_UsernameTextbox, "", wxPoint(100, 30), wxSize(300, 50));
//     username_textbox->Destroy();
//     // Bind(wxEVT_TEXT, &MyFrame::OnRegister, this, ID_UsernameTextbox);
//     Bind(wxEVT_BUTTON, &RegisterFrame::OnRegister, this, ID_RegisterButton);
// }

// RegisterFrame::~RegisterFrame()
// {
// }

// void RegisterFrame::OnExit(wxCommandEvent &event)
// {
//     Close(true);
// }

// void RegisterFrame::OnAbout(wxCommandEvent &event)
// {
//     wxMessageBox("This is a wxWidgets Hello World example",
//                  "About Hello World", wxOK | wxICON_INFORMATION);
// }

// void RegisterFrame::OnHello(wxCommandEvent &event)
// {
//     wxLogMessage("Hello world from wxWidgets!");
// }
// void RegisterFrame::OnRegister(wxCommandEvent &event)
// {
//     wxTextCtrl *username_textbox = (wxTextCtrl *)FindWindow(ID_UsernameTextbox);
//     wxString username = username_textbox->GetValue();
//     // wxLogMessage("Username: %s", username);

//     // Check user name with regex
//     // If valid, send to server
//     // If invalid, display error message
//     // Valid username contains alphanumeric characters and underscores only,maximum length of 10 characters
//     // std::regex username_regex("^[a-zA-Z0-9_]{1,10}$");
//     // if (std::regex_match(username.ToStdString(), username_regex))
//     // {
//     //     wxMessageBox("Username is valid\nUsername: " + username, "Username is valid", wxOK | wxICON_INFORMATION);
//     // }
//     // else
//     // {
//     //     wxMessageBox("Username is invalid");
//     // }
//     this->sendMessage("REGISTER " + username.ToStdString());

//     // wxLogMessage("Register button clicked!");
// }