/*
Bastian Ruppert
*/

namespace EuMax01
{
  //  #include "pec_cmd.h"
#ifndef __ERRORDIALOG_H__
#define __ERRORDIALOG_H__

  class ArbeitsDialog;

  class ErrorDialog : public Screen
  {
  public:
    ErrorDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    void setErrorMsg(char * Message);
  private:
    Label * Label_Error;
    Label * Label_Info;
    Label * Label_OK;
  };

  class ConfirmDialog : public Screen
  {
  public:
    ConfirmDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    void setConfirmMsg(char * Message);
  private:
    Label * Label_Info;
    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
  };

  class SplashScreen : public Screen
  {
  public:
    SplashScreen(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ~SplashScreen();
    ArbeitsDialog * Parent;
    void showCrossPosition(int cam,int wert);
  private:
    Label * Label_Info;
    Label * Label_CrossPosLeft;
    Label * Label_CrossPosRight;
    char pcCrossPosLeft[128];
    char pcCrossPosRight[128];
    Button * pButtonLogo;
    SDL_Surface * pLogo;
    int loadImage(char * path);
    char Versionsnummer[1024];
  };
 
#endif /* __ERRORDIALOG_H__*/
}//end namespace
