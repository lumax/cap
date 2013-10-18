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
    static void return_listener(void * src, SDL_Event * evt);

  protected:
    Label * Label_Error;
    Label * Label_Info;
    Button * Button_OK;
  };

  class FlexibleErrorDialog : public ErrorDialog
  {
  public:
    FlexibleErrorDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
			ArbeitsDialog * parent);
    void setReturnDialogID(int id);
    int getReturnDialogID();
    static void flexible_return_listener(void * src, SDL_Event * evt);
  private:
    int DialogID;
  };

 class FlexibleInfoDialog : public FlexibleErrorDialog
  {
  public:
    FlexibleInfoDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
			ArbeitsDialog * parent);
  private:
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

  class OkCancelDialog : public Screen
  {
  public:
    OkCancelDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    void setMsg(char * Message);
    void setHeadline(char * headline);

    void setDialogSource(void * source);
    void * getDialogSource();

    static void OkCancelDialogKeyListener(void * src, SDL_Event * evt);
    static void return_listener(void * src, SDL_Event * evt);
    static void esc_listener(void * src,SDL_Event * evt);

    void setOKFnk(void (*OKFnk)(void * src, SDL_Event * evt));
    void setCancelFnk(void (*CancelFnk)(void * src, SDL_Event * evt));

    void (*OKFnk)(void * src, SDL_Event * evt);
    void (*CancelFnk)(void * src, SDL_Event * evt);

  protected:
    void * src;
    int DialogID;
    Label * Label_Error;
    Label * Label_Info;
    Button * Button_OK;
    Button * Button_Cancel;
  };

#endif /* __ERRORDIALOG_H__*/
}//end namespace
