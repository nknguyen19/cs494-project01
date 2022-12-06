#include "myframe.h"

/*****************************************************
 * 
 *  Utility functions
 * 
 * ***************************************************/
MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Hello World")
{
    // connectServer();
    this->SetBackgroundColour(wxColour(*wxBLACK));
    this->game = nullptr;
    this->SetBackgroundColour(*wxLIGHT_GREY);
    this->SetForegroundColour(*wxWHITE);
    this->timer = new wxTimer(this, ID_Timer);
    this->timerText = NULL;
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
    Bind(wxEVT_TIMER, &MyFrame::OnTimer, this, ID_Timer);
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
        this->SetSize(800, 600);

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

    if (this->timer != NULL && this->timer->IsRunning()) {
        this->timer->Stop();
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
    if (this->timer != NULL && this->timer->IsRunning()) {
        this->timer->Stop();
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
    if (this->game && !this->game->isFinished()) {
        wxLogMessage("Game is not finished");
        return;
    }
    if (this->connectServer())
    {
        this->showRegisterFrame();
    }
}

void MyFrame::OnTimer(wxTimerEvent &event)
{
    this->time -= 1;
    if (this->time == 0) {
        this->timer->Stop();
        wxLogMessage("Time is up!");
        this->sendMessage("ANSWER TIMEOUT");
    }
    else {
        this->timer->Start(1000);
        this->showGameFrame();
    }
}

void MyFrame::handleCorrectAnswer()
{
    // wxLogStatus("Correct answer! Continue to next question");
    // wxLogMessage("Correct answer! Continue to next question");
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
    this->SetSize(500, 300);
    wxStaticText *helloText = new wxStaticText(this, ID_Hello, "Hello, Welcome to\nWHO WANTS TO BE A MILLIONAIRE!", wxPoint(10, 10), wxSize(500, 60));
    helloText->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    helloText->SetForegroundColour(wxColour(0, 0, 0));
    controls.push_back(helloText);
    wxButton *startButton = new wxButton(this, ID_StartButton, "Start", wxPoint(200, 100), wxSize(100, 20));
    startButton->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
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

void MyFrame::displayPlayers() {
    vector<Player*> players = this->game->getPlayers();
    for (int i = 0; i < players.size(); i++)
    {
        Player *player = players[i];
        wxButton *player_name = new wxButton(this, 
                                            ID_PlayerNickname, 
                                            player->nickname, 
                                            wxPoint(50, 100 + i * 55), 
                                            wxSize(200, 50),
                                            wxBORDER_NONE);
        player_name->SetBackgroundColour(wxColor(26, 26, 26));
        player_name->SetForegroundColour(wxColor(255, 255, 255));
        player_name->SetWindowStyle(wxBORDER_DOUBLE);
        player_name->SetFont(wxFont(15, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD));
        controls.push_back(player_name);

        if (player->status == Game::PLAYER_DISQUALIFIED){
            // display eliminated icon
            wxStaticText *eliminated_icon = new wxStaticText(this, 
                                                            ID_EliminatedIcon, 
                                                            "X", 
                                                            wxPoint(200, 110 + i * 55), 
                                                            wxSize(50, 50));
            eliminated_icon->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT));
            eliminated_icon->SetForegroundColour(wxColor(255, 0, 0));
            controls.push_back(eliminated_icon);
        }
    }
    wxStaticText *numberOfPlayer = new wxStaticText(
        this, 
        ID_NumberOfPlayer, 
        "Number of player(s): " + 
        to_string(this->game->getNumberOfPlayingPlayers()) + "/" + to_string(this->game->getNumberOfPlayers()), 
        wxPoint(30, 470), 
        wxSize(200, 50)
    );
    numberOfPlayer->SetForegroundColour(wxColour(0, 0, 0));
    controls.push_back(numberOfPlayer);
}

void MyFrame::displayQuestion() {
    Question *question = this->game->getCurrentQuestion();
    wxBoxSizer *questionBox = new wxBoxSizer(wxVERTICAL);
    wxStaticText *question_text = new wxStaticText(this, ID_Question, question->question, wxPoint(300, 100), wxSize(450, 100));
    question_text->SetFont(wxFont(18, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    question_text->SetForegroundColour(wxColor(26, 26, 26));
    // questionBox->Add(question_text, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
    controls.push_back(question_text);

    // display answers
    vector<string> answers = question->answers;
    for (int i = 0; i < answers.size(); i++)
    {
        wxButton *answer_button = new wxButton(this, ID_Answer + i, answers[i], wxPoint(300, 200 + 55 * i), wxSize(400, 50));
        answer_button->SetBackgroundColour(wxColor(26, 26, 26));
        answer_button->SetForegroundColour(wxColor(255, 255, 255));
        answer_button->SetFont(wxFont(10, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT));
        // questionBox->Add(answer_button, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
        controls.push_back(answer_button);
        Bind(wxEVT_BUTTON, &MyFrame::OnAnswer, this, ID_Answer + i);
    }

    wxStaticText *numberOfQuestion = new wxStaticText(
        this, 
        ID_NumberOfQuestion, 
        "Question " + to_string(this->game->getCurrentQuestionId() + 1) + "/" + to_string(this->game->getNumberOfQuestions()), 
        wxPoint(430, 470), 
        wxSize(200, 50)
    );
    numberOfQuestion->SetForegroundColour(wxColour(0, 0, 0));
    controls.push_back(numberOfQuestion);
}

void MyFrame::displayeStatus() {
    if (!this->game->isFinished()) {
        string currentPlayingPlayer = this->game->getCurrentPlayingPlayer().nickname;
        wxStaticText *myStatusText = new wxStaticText(this, 
                                                    ID_MyStatus, 
                                                    this->game->isWaiting() ? "Waiting for other players" :
                                                    (this->game->getIsMyTurn() ? "Your turn" : (currentPlayingPlayer + "'s turn")),
                                                    wxPoint(50, 50), 
                                                    wxSize(400, 50));
        myStatusText->SetFont(wxFont(20, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        myStatusText->SetForegroundColour(wxColor(26, 26, 26));
        controls.push_back(myStatusText);

        // display move turn button
        if (this->game->getMyPlayer().canMoveTurn)
        {
            wxButton *moveButton = new wxButton(this, ID_MoveButton, "Move", wxPoint(650, 30), wxSize(100, 50));
            controls.push_back(moveButton);
            Bind(wxEVT_BUTTON, &MyFrame::OnMoveTurn, this, ID_MoveButton);
        }
        if (this->game->getIsMyTurn() && (this->timer == NULL || !this->timer->IsRunning())) {
            // display timer
            this->timer->Start(1000);
            this->time = 10;
        }
    }

    if (this->game->isFinished()) {
        // display result
        wxStaticText *result_text = new wxStaticText(this, ID_Result, this->game->getResult(), wxPoint(200, 50), wxSize(500, 50));
        result_text->SetFont(wxFont(20, wxFONTFAMILY_MODERN, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD));
        result_text->SetForegroundColour(wxColor(51, 102, 0));
        controls.push_back(result_text);
    }

    if (this->game->isEliminated()) {
        // display eliminated
        wxStaticText *eliminated_text = new wxStaticText(this, ID_Eliminated, "You have been eliminated!", wxPoint(200, 500), wxSize(400, 50));
        eliminated_text->SetFont(wxFont(10, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_LIGHT));
        eliminated_text->SetForegroundColour(wxColor(255, 0, 0));
        controls.push_back(eliminated_text);
    }
}

void MyFrame::showGameFrame()
{
    // display timer
    if (this->timerText != NULL) {
        this->timerText->Destroy();
        this->timerText = NULL;
    }

    if (this->timer && this->timer->IsRunning()) {
        this->timerText = new wxStaticText(
            this, 
            ID_TimerText, 
            to_string(this->time), 
            wxPoint(500, 50), 
            wxSize(100, 50)
        );
        this->timerText->SetFont(wxFont(20, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        this->timerText->SetForegroundColour(wxColor(255, 0, 0));
    }
    else {
        clearFrame();

        // display my status
        this->displayeStatus();
        // display players
        this->displayPlayers();

        if (this->game->isPlaying()) {
            // display question
            this->displayQuestion();
        }
    }

    string response;
    if (!this->receiveMesage(response)) {
        wxLogMessage("Failed to connect to server");
        return;
    }
    this->game->update(response);
    this->showGameFrame();
}
