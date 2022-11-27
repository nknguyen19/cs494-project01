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
    menuFile->Append(ID_Hello, "&Home\tCtrl-H",
                     "Back to home screen");
    menuFile->AppendSeparator();
    menuFile->Append(ID_NewGame, "&New game\tCtrl-N",
                    "Start a new game");
    menuFile->AppendSeparator();
    menuFile->Append(ID_LogOut, "&Log out\tCtrl-L",
                     "Log out from this game");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT), "&Exit\tCtrl-Q",
        "Quit this program";

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
    Bind(wxEVT_MENU, &MyFrame::OnNewGame, this, ID_NewGame);
    Bind(wxEVT_MENU, &MyFrame::OnLogout, this, ID_LogOut);
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
    if (client->WaitOnConnect(5))
    {
        string result;
        if (receiveMesage(result))
        {
            if (result == OK)
            {
                return true;
            }
        }
    }
    wxLogMessage("Failed to connect to server");
    return false;
}

void MyFrame::sendMessage(string message)
{
    if (!client->IsConnected())
    {
        wxLogMessage("Not connected to server");
        showGreetingsFrame();
        return;
    }
    int res = (client->Write(message.c_str(), message.length())).LastCount();
}

bool MyFrame::receiveMesage(string &message)
{
    if (!client->IsConnected())
    {
        return false;
    }
    this->client->WaitForRead();
    char buffer[1024];
    int res = (client->Read(buffer, 1024)).LastCount();
    message = string(buffer, res);
    return true;
}


string MyFrame::pasreMessageStatus(string message)
{
    return message.substr(0, 3);
}

string MyFrame::parseMessageContent(string message)
{
    return message.substr(4, message.find_first_of("\n") - 4);
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
    //force close frame
    Close(true);

    // stop waiting for server message
    this->client->InterruptWait();
    this->client->Close();
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
    string response;
    if (!this->receiveMesage(response))
    {
        wxLogMessage("Failed to connect to server");
        return;
    }
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
        if (response.length() == 0)
        {
            // not sure sometimes game status is not sent along with the previous message
            if (!this->receiveMesage(response))
            {
                wxLogMessage("Failed to connect to server");
                return;
            }
        }
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

void MyFrame::OnAnswer(wxCommandEvent &event)
{
    if (!this->game->getIsMyTurn()) {
        wxLogMessage("It's not your turn");
        return;
    }
    if (this->game->isFinished()) {
        wxLogMessage("Game is finished");
        return;
    }
    if (this->game->isEliminated()) {
        wxLogMessage("You are eliminated");
        return;
    }
    int id = event.GetId();
    int answer = id - ID_Answer;
    this->sendMessage("ANSWER " + string(1, char(answer + 'A')));
    string response;
    if (!this->receiveMesage(response))
    {
        wxLogMessage("Failed to connect to server");
        return;
    }
    
    // Answer result
    string status = this->pasreMessageStatus(response);
    string content = this->parseMessageContent(response);
    string game_status = response.substr(response.find_first_of("\n") + 1);
    if (game_status.length() == 0) {
        if (!this->receiveMesage(game_status))
        {
            wxLogMessage("Failed to connect to server");
            return;
        }
    }
    this->game->update(game_status);
    this->game->update(game_status);
    if (status == "200") {
        if (content == CORRECT) {
            this->handleCorrectAnswer();
        } 
        else if (content == INCORRECT) {
            this->handleWrongAnswer();
        }
        this->showGameFrame();
    }
    else {
        wxLogMessage(content.c_str());
    }
}

void MyFrame::OnMoveTurn(wxCommandEvent &event)
{
    if (!this->game->getIsMyTurn()) {
        wxLogMessage("It's not your turn");
        return;
    }
    if (this->game->isFinished()) {
        wxLogMessage("Game is finished");
        return;
    }
    if (this->game->isEliminated()) {
        wxLogMessage("You are eliminated");
        return;
    }
    this->sendMessage("MOVE");
    string response;
    if (!this->receiveMesage(response))
    {
        wxLogMessage("Failed to connect to server");
        return;
    }
    string status = this->pasreMessageStatus(response);
    string content = this->parseMessageContent(response);
    string game_status = response.substr(response.find_first_of("\n") + 1);
    if (game_status.length() == 0) {
        if (!this->receiveMesage(game_status))
        {
            wxLogMessage("Failed to connect to server");
            return;
        }
    }
    this->game->update(game_status);
    if (status == "200") {
        this->showGameFrame();
        wxLogStatus("Moved your turn!");
    }
    else {
        wxLogMessage(content.c_str());
    }
}

void MyFrame::OnLogout(wxCommandEvent &event)
{
    this->sendMessage("LOGOUT");
    string response;
    if (!this->receiveMesage(response))
    {
        wxLogMessage("Failed to connect to server");
        return;
    }
    string status = this->pasreMessageStatus(response);
    string content = this->parseMessageContent(response);
    if (status == "200") {
        wxLogMessage("Logout successfully");
        this->showGreetingsFrame();
    }
    else {
        wxLogMessage(content.c_str());
    }
}

void MyFrame::OnNewGame(wxCommandEvent &event)
{
    if (!this->game->isFinished()) {
        wxLogMessage("Game is not finished");
        return;
    }
    this->showRegisterFrame();
}

void MyFrame::handleCorrectAnswer()
{
    // wxLogStatus("Correct answer! Continue to next question");
    wxLogMessage("Correct answer! Continue to next question");
}

void MyFrame::handleWrongAnswer()
{
    // wxLogStatus("Incorrect answer! You have been eliminated!");
    wxLogMessage("Incorrect answer! You have been eliminated!");
    
}

/******************************************************
 * 
 *  Show Frame
 * 
 ******************************************************/

void MyFrame::showGreetingsFrame()
{
    clearFrame();
    this->SetSize(400, 300);
    wxStaticText *helloText = new wxStaticText(this, ID_Hello, "Hello, ", wxPoint(10, 10), wxSize(100, 20));
    controls.push_back(helloText);
    wxButton *startButton = new wxButton(this, ID_StartButton, "Start", wxPoint(10, 40), wxSize(100, 20));
    controls.push_back(startButton);
    Bind(wxEVT_BUTTON, &MyFrame::OnStart, this, ID_StartButton);
}

void MyFrame::showRegisterFrame()
{
    clearFrame();
    this->SetSize(500, 300);
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

    // display my status
    if (!this->game->isFinished()){
        string currentPlayingPlayer = this->game->getCurrentPlayingPlayer().nickname;
        wxStaticText *myStatusText = new wxStaticText(this, 
                                                    ID_MyStatus, 
                                                    this->game->isWaiting() ? "Waiting for other player" :
                                                    (this->game->getIsMyTurn() ? "Your turn" : (currentPlayingPlayer + "'s turn")),
                                                    wxPoint(250, 50), 
                                                    wxSize(200, 50));
        controls.push_back(myStatusText);

        // display move turn button
        if (this->game->getMyPlayer().canMoveTurn)
        {
            wxButton *moveButton = new wxButton(this, ID_MoveButton, "Move", wxPoint(650, 30), wxSize(100, 50));
            controls.push_back(moveButton);
            Bind(wxEVT_BUTTON, &MyFrame::OnMoveTurn, this, ID_MoveButton);
        }
    }

    // display players
    vector<Player*> players = this->game->getPlayers();
    for (int i = 0; i < players.size(); i++)
    {
        Player *player = players[i];
        wxStaticText *player_name = new wxStaticText(this, ID_PlayerNickname, player->nickname, wxPoint(100, 150 + 50 * i), wxSize(100, 50));
        controls.push_back(player_name);
    }

    if (this->game->isPlaying()) {
        // display question
        Question *question = this->game->getCurrentQuestion();
        wxStaticText *question_text = new wxStaticText(this, ID_Question, question->question, wxPoint(250, 150), wxSize(500, 50));
        controls.push_back(question_text);

        // display answers
        vector<string> answers = question->answers;
        for (int i = 0; i < answers.size(); i++)
        {
            wxButton *answer_button = new wxButton(this, ID_Answer + i, answers[i], wxPoint(250, 200 + 50 * i), wxSize(500, 50));
            controls.push_back(answer_button);
            Bind(wxEVT_BUTTON, &MyFrame::OnAnswer, this, ID_Answer + i);
        }
    }

    if (this->game->isFinished()) {
        // display result
        wxStaticText *result_text = new wxStaticText(this, ID_Result, this->game->getResult(), wxPoint(200, 50), wxSize(500, 50));
        controls.push_back(result_text);
    }

    if (this->game->isEliminated()) {
        // display eliminated
        wxStaticText *eliminated_text = new wxStaticText(this, ID_Eliminated, "You have been eliminated!", wxPoint(200, 500), wxSize(400, 50));
        controls.push_back(eliminated_text);
    }
    string response;
    if (!this->receiveMesage(response)) {
        wxLogMessage("Failed to connect to server");
        return;
    }
    this->game->update(response);
    this->showGameFrame();
}
