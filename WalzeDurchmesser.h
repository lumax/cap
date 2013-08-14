/*
Bastian Ruppert
*/

namespace EuMax01
{
#ifndef __WALZEDURCHMESSER_H__
#define __WALZEDURCHMESSER_H__

  class WalzeDurchmesser : public Screen
  {
  public:
    WalzeDurchmesser(int sdlw,int sdlh, int camw,int camh,int yPos,\
		      NewDialog * parent);
    ~WalzeDurchmesser();
    NewDialog * Parent;

    void useWalzeDurchmesser(int durchmesser);
    void showEingabeSchritt();

    int derDurchmesser;

    static void WalzeDurchmesserKeyListener(void * src, SDL_Event * evt);

    static void escape_listener(void * src, SDL_Event * evt);
    static void return_listener(void * src, SDL_Event * evt);

  private:
    void getSchritteValues(char * buf,int len);

    Label * Label_Step;
    char StepText[256];

    Label * Label_ValueName;
    char ValueName[16];

    Label * Label_OldValue;
    char OldValue[64];

    TextField * TF_Value;
    const int TF_Len; 
    char Value[64];

    void confirmValue(int val);
    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
  };
  
#endif /* __WALZEDURCHMESSER_H__*/
}//end namespace
