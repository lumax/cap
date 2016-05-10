/*
Bastian Ruppert
*/

namespace EuMax01
{
  //  #include "pec_cmd.h"
#ifndef __G_TESTDIALOG_H__
#define __G_TESTDIALOG_H__

  class ArbeitsDialog;

  class G_TestDialog : public Screen
  {
  public:
    G_TestDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
		      ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    //void setQ1(unsigned int dat);
    //void setQ2(unsigned int dat);
    //void setZ1(unsigned int dat,char*suffix,unsigned int zFaktor);
    //void setZ2(unsigned int dat,char*suffix);
    //void resetStepValue();
    //void incStep();
    //void decStep();
    //void showStep(int theNewStep);

    static void escape_listener(void * src, SDL_Event * evt);
    static void left_listener(void * src, SDL_Event * evt);
    static void right_listener(void * src, SDL_Event * evt);
    static void return_listener(void * src, SDL_Event * evt);

  private:
    Label * Label_Step;
    char StepText[256];

    Label * Label_ValueName;
    char ValueName[16];

    Label * Label_Value;
    char Value[64];

    int ActualStep;
    //void setXXData(unsigned int dat,int MyStep,char * suffix);
    //void setXXData(unsigned int dat,int MyStep,char * suffix,unsigned int zFaktor);

    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;

  protected:

  };

#endif /* __G_TESTDIALOG_H__*/
}//end namespace
