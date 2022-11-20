#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <string>
#include <regex>

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
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

