#include "myapp.h"
#include <string>

using namespace std;

bool MyApp::OnInit()
{
    connectSerever();
    MyFrame *frame = new MyFrame(this->client);
    frame->SetSize(500, 250);
    frame->Show(true);
    return true;
}

void MyApp::connectSerever()
{
    // init socket
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

    // wait for connection
    // client->WaitOnConnect();

    // read response
    char buffer[1024];
    client->Read(buffer, 1024);
    string response(buffer);
}

void MyApp::sendMessage(string message)
{
    client->Write(message.c_str(), message.length());
}

string MyApp::receiveMessage()
{
    char buffer[1024];
    client->Read(buffer, 1024);
    string response(buffer);
    return response;
}
