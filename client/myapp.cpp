#include "myapp.h"
#include <string>

using namespace std;

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->SetSize(500, 250);
    frame->Show(true);
    return true;
}
