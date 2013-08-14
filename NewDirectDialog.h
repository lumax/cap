/*
Bastian Ruppert
*/

namespace EuMax01
{
  //  #include "pec_cmd.h"
#ifndef __NEWDIRECTDIALOG_H__
#define __NEWDIRECTDIALOG_H__

  //class ArbeitsDialog;
  //  class NewDialog;

  class NewDirectDialog : public Screen
  {
  public:
    NewDirectDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
		      NewDialog * parent);
    ~NewDirectDialog();
    NewDialog * Parent;
    //void setQ1(unsigned short dat);
    //void setQ2(unsigned short dat);
    //void setZ1(unsigned short dat);
    //void resetStepValue();
    void incEingabeSchritt();
    void decEingabeSchritt();
    int getEingabeSchritt();
    void showEingabeSchritt();
    PositionSet * getPositionSet();
    void useNewDirectDialog(PositionSet * thePositionSet,int theStep);

    static void NewDirectKeyListener(void * src, SDL_Event * evt);
    //void crosshairsKeyListener(SDL_KeyboardEvent * key);

    static void escape_listener(void * src, SDL_Event * evt);
    static void left_listener(void * src, SDL_Event * evt);
    static void right_listener(void * src, SDL_Event * evt);
    static void return_listener(void * src, SDL_Event * evt);

  private:
    int theStep;
    static const int AnzahlEingabeSchritte = 3;//camx1,camx2,z,/*cross1,cross2*/
    char * SchrittTexte[NewDirectDialog::AnzahlEingabeSchritte];
    void getSchritteValueNames(char * buf,int len);
    void getSchritteValues(char * buf,int len);
    //PositionSet * thePositionSet;
    Label * Label_Step;
    char StepText[256];

    Label * Label_ValueName;
    char ValueName[16];

    Label * Label_OldValue;
    char OldValue[64];

    TextField * TF_Value;
    const int TF_Len; 
    char Value[64];

    //Label * Label_Menu;
    
    //Label * Label_MenuTitle;

    static const int iQ1 = 0;
    static const int iQ2 = 1;
    static const int iZ1 = 2;
    static const int iZ2 = 3;

    int ActualStep;
    PositionSet thePosSet;
    void setXXData(unsigned short dat,int MyStep,char * suffix);
    void confirmValue(int val);
    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
  };
  
#endif /* __NEWDIRECTDIALOGE_H__*/
}//end namespace
