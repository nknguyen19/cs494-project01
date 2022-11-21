#include "myframe.h"

/*****************************************************
 * 
 *  Utility functions
 * 
 * ***************************************************/
MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Hello World")
{
    // connectServer();
    this->game = nullptr;
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
    showGreetingsFrame();

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

MyFrame::~MyFrame()
{
}

bool MyFrame::connectServer()
{
    client = new wxSocketClient();

    // set event handler
    client->SetEventHandler(*this);
    client->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    client->Notify(true);

    // connect to server
    wxIPV4address addr;
    addr.Hostname("localhost");
    addr.Service(PORT);
    client->Connect(addr, false);
    string result = receiveMesage();
    if (result == OK)
    {
        return true;
    }
    else
    {
        wxLogMessage("Failed to connect to server");
        return false;
    }
}

void MyFrame::sendMessage(string message)
{
    int res = (client->Write(message.c_str(), message.length())).LastCount();
}

string MyFrame::receiveMesage()
{
    this->client->WaitForRead();
    char buffer[1024];
    int res = (client->Read(buffer, 1024)).LastCount();
    return string(buffer, res);
}


string MyFrame::pasreMessageStatus(string message)
{
    return message.substr(0, 3);
}

string MyFrame::parseMessageContent(string message)
{
    return message.substr(4, message.length() - 4);
}

string MyFrame::parseGameStatus(string message)
{
    return message.substr(0, 1);
}

void MyFrame::clearFrame()
{
    for (wxControl *control : controls)
    {
        control->Destroy();
    }
    controls.clear();
}

/******************************************************
 * 
 *  Event Handler
 * 
 ******************************************************/
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
    // Send message to register with username 
    wxTextCtrl *username_textbox = (wxTextCtrl *)FindWindow(ID_UsernameTextbox);
    wxString username = username_textbox->GetValue();
    this->sendMessage("REGISTER " + string(username));

    // Receive message from server
    string response = this->receiveMesage();
    wxLogMessage("Response: %s", response);

    // Process response
    string status = this->pasreMessageStatus(response);
    string content = this->parseMessageContent(response);
    if (status == "200")
    {
        wxLogMessage("Register successfully");
        this->game = new Game(string(username));
        // TODO : Show game frame
        response = response.substr(response.find_first_of("\n") + 1);
        this->game->update(response);
        this->showGameFrame();
    }
    else
    {
        wxLogMessage("Register failed: %s", content);
    }
}

void MyFrame::OnStart(wxCommandEvent &event)
{
    if (this->connectServer())
    {
        this->showRegisterFrame();
    }
}


/******************************************************
 * 
 *  Show Frame
 * 
 ******************************************************/

void MyFrame::showGreetingsFrame()
{
    clearFrame();
    wxStaticText *helloText = new wxStaticText(this, ID_Hello, "Hello, ", wxPoint(10, 10), wxSize(100, 20));
    controls.push_back(helloText);
    wxButton *startButton = new wxButton(this, ID_StartButton, "Start", wxPoint(10, 40), wxSize(100, 20));
    controls.push_back(startButton);
    Bind(wxEVT_BUTTON, &MyFrame::OnStart, this, ID_StartButton);
}

void MyFrame::showRegisterFrame()
{
    clearFrame();
    wxButton *registerButton = new wxButton(this, ID_RegisterButton, "Register", wxPoint(200, 100), wxSize(100, 50));
    controls.push_back(registerButton);
    wxTextCtrl *username_textbox = new wxTextCtrl(this, ID_UsernameTextbox, "", wxPoint(100, 30), wxSize(300, 50));
    controls.push_back(username_textbox);
    Bind(wxEVT_BUTTON, &MyFrame::OnRegister, this, ID_RegisterButton);
}

void MyFrame::showGameFrame()
{
    clearFrame();
    this->SetSize(800, 600);

    // display players
    vector<Player*> players = this->game->getPlayers();
    for (int i = 0; i < players.size(); i++)
    {
        Player *player = players[i];
        wxStaticText *player_name = new wxStaticText(this, ID_PlayerNickname, player->nickname, wxPoint(50, 50 + 50 * i), wxSize(100, 50));
        controls.push_back(player_name);
    }

    if (this->game->isPlaying()) {
        // display question
        Question *question = this->game->getCurrentQuestion();
        wxStaticText *question_text = new wxStaticText(this, ID_Question, question->question, wxPoint(200, 50), wxSize(500, 50));
        controls.push_back(question_text);

        // display answers
        vector<string> answers = question->answers;
        for (int i = 0; i < answers.size(); i++)
        {
            wxButton *answer_button = new wxButton(this, ID_Answer + i, answers[i], wxPoint(200, 100 + 50 * i), wxSize(500, 50));
            controls.push_back(answer_button);
            Bind(wxEVT_BUTTON, &MyFrame::OnAnswer, this, ID_Answer + i);
        }
    }

    string response = this->receiveMesage();
    this->game->update(response);
    if (!this->game->isFinished()) {
        this->showGameFrame();
    }
}

void MyFrame::OnAnswer(wxCommandEvent &event)
{
    int id = event.GetId();
    int answer = id - ID_Answer;
    this->sendMessage("ANSWER " + string(1, char(answer + 'A')));
    string response = this->receiveMesage();
}