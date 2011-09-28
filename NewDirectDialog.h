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
    void showEingabeSchritt();
    void useNewDirectDialog(PositionSet * thePositionSet);
    
    void * getKeyListener();

  private:
    static const int AnzahlEingabeSchritte = 5;//camx1,camx2,z,cross1,cross2
    char * SchrittTexte[NewDirectDialog::AnzahlEingabeSchritte];

    PositionSet * thePositionSet;
    Label * Label_Step;
    char StepText[256];

    Label * Label_ValueName;
    char ValueName[16];

    TextField * TF_Value;
    const int TF_Len; 
    char Value[64];

    Label * Label_Menu;
    char InfoText[256];
    
    Label * Label_MenuTitle;

    static const int iQ1 = 0;
    static const int iQ2 = 1;
    static const int iZ1 = 2;
    static const int iZ2 = 3;

    int ActualStep;
    void setXXData(unsigned short dat,int MyStep,char * suffix);
  };
  
#endif /* __NEWDIRECTDIALOGE_H__*/
}//end namespace
