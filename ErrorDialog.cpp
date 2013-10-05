/*
Bastian Ruppert
*/
//read directories
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <termios.h>

#include <string.h>

#include <errno.h>

#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "LL.h"
#include "Event.h"
#include "Tools.h"
#include "Globals.h"
#include "Button.h"
#include "Poll.h"
#include "Label.h"
#include "Screen.h"
#include "Main.h"

#include "v4l_capture.h"
#include "Rezept.h"
#include "Protocol.h"
#include "pec_cmd.h"
#include "MBProt.h"

#include "MenuBar.h"
#include "ArbeitsDialog.h"
#include "NewDirectDialog.h"
#include "Version.h"

#include "ErrorDialog.h"

namespace EuMax01
{

  static void ErrorDialogKeyListener(void * src, SDL_Event * evt)
  {
    ErrorDialog* ad = (ErrorDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->return_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->return_listener(src,evt);
	  }
      }
  }

  void ErrorDialog::return_listener(void * src, SDL_Event * evt)
  {
    ErrorDialog* ad = (ErrorDialog*)src;//KeyListener
    ad->Parent->showArbeitsDialog();
  }

  ErrorDialog::ErrorDialog(int sdlw,		\
			   int sdlh,		\
			   int camw,		\
			   int camh,					\
			   int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    short Zeile5_y;

    this->Parent = parent;

    M_y = sdlh - yPos;

    if(M_y<=84)
      {
	MSpace_h = 2;
	MZeile_h = 18;
      }
    else
      {
	MSpace_h = 5;
	MZeile_h = 28;
      }

    MLinks_x = sdlw/2 - 506;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;


    //MLoadName_y  = yPos + 1*MSpace_h + 0*MZeile_h;

    Label_Error = new Label("Error",					\
			    MLinks_x,					\
			    yPos + 1*MSpace_h + 0*MZeile_h,		\
			    506*2,MZeile_h,Parent->AlertSet);
    
    Label_Info = new Label("----",					\
			   MLinks_x,					\
			   yPos + 2*MSpace_h + 1*MZeile_h,		\
			   506*2,MZeile_h,Parent->MenuSet);
    
    Button_OK = new Button("OK  (Enter)",					\
			 MLinks_x,				\
			 Zeile5_y,		\
			 506*2,MZeile_h,		\
			 Parent->MenuSet);

    this->Button_OK->setLMButtonUpEvtHandler(return_listener);
    this->Button_OK->pTSource = this;
    
    this->pTSource = this;//EvtTarget Quelle setzen
    this->EvtTargetID=(char*)"ErrorDialog";
    this->setKeyboardUpEvtHandler(ErrorDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    this->addEvtTarget(Label_Error);
    this->addEvtTarget(Label_Info);
    this->addEvtTarget(Button_OK);
  }

  void ErrorDialog::setErrorMsg(char * Message)
  {
    this->Label_Info->setText(Message);
  }

  static void FlexibleErrorDialogKeyListener(void * src, SDL_Event * evt)
  {
    FlexibleErrorDialog* ad = (FlexibleErrorDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE || \
	   key->keysym.sym == SDLK_RETURN || \
	   key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->flexible_return_listener(src,evt);
	  }
      }
  }

  void FlexibleErrorDialog::flexible_return_listener(void * src, SDL_Event * evt)
  {
    FlexibleErrorDialog* ad = (FlexibleErrorDialog*)src;//KeyListener
    if(ad->getReturnDialogID()==ArbeitsDialog::BackupMenuDialogIsActive)
      ad->Parent->showBackupMenuDialog();
    else if(ad->getReturnDialogID()==ArbeitsDialog::CreateBackupDialogIsActive)
      ad->Parent->showCreateBackupDialog();
    else
      ad->Parent->showArbeitsDialog();
  }

  FlexibleErrorDialog::FlexibleErrorDialog(int sdlw,	\
				   int sdlh,	\
				   int camw,				\
				   int camh,				\
				   int yPos,ArbeitsDialog * parent):\
    ErrorDialog(sdlw,sdlh,camw,camh,yPos,parent)
  {
    this->Button_OK->setLMButtonUpEvtHandler(flexible_return_listener);
    this->Label_Error->setText("Error");
    this->setKeyboardUpEvtHandler(FlexibleErrorDialogKeyListener);
    this->EvtTargetID=(char*)"FlexlibleErrorDialog";
  }

  void FlexibleErrorDialog::setReturnDialogID(int id)
  {
    this->DialogID=id;
  }

  int FlexibleErrorDialog::getReturnDialogID()
  {
    return this->DialogID;
  }

  FlexibleInfoDialog::FlexibleInfoDialog(int sdlw,	\
				   int sdlh,	\
				   int camw,				\
				   int camh,				\
				   int yPos,ArbeitsDialog * parent):\
    FlexibleErrorDialog(sdlw,sdlh,camw,camh,yPos,parent)
  {
    this->Label_Error->setText("Info");
    this->EvtTargetID=(char*)"FlexlibleInfoDialog";
  }


  static void ConfirmEnter(void * src, SDL_Event * evt)
  {
    exit(12);
  }

  static void ConfirmService(void * src, SDL_Event * evt)
  {
    exit(42);
  }
  
  static void ConfirmEscape(void * src, SDL_Event * evt)
  {
    ConfirmDialog* ad = (ConfirmDialog*)src;
    ad->Parent->showArbeitsDialog();
  }

  static void ConfirmF2(void * src, SDL_Event * evt)
  {
    ConfirmDialog* ad = (ConfirmDialog*)src;
    ad->Parent->showInfoDialog();
  }

  static void ConfirmDialogKeyListener(void * src, SDL_Event * evt)
  {
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ConfirmEscape(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ConfirmEnter(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F1)
	  {
	    ConfirmService(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F2)
	  {
	    ConfirmF2(src,evt);
	  }
      }
  }

  ConfirmDialog::ConfirmDialog(int sdlw,		\
			     int sdlh,		\
			     int camw,					\
			     int camh,					\
			     int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y;
    short Spalte1_x, Spalte2_x, Spalte3_x;

    short Button_w = 332;
    short x_space = 8;

    this->Parent = parent;

    M_y = sdlh - yPos;

    if(M_y<=84)
      {
	MSpace_h = 2;
	MZeile_h = 18;
      }
    else
      {
	MSpace_h = 5;
	MZeile_h = 28;
      }

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    Spalte1_x = sdlw/2 - 506;
    Spalte2_x = Spalte1_x + 1*Button_w+1*x_space;
    Spalte3_x = Spalte1_x + 2*Button_w+2*x_space;

    Label_Info = new Label("---",\
			   Spalte1_x,\
			   Zeile1_y,\
			   506*2,\
			   MZeile_h,\
			   Parent->DialogSet);

    theMenuBarSettings.Text[0]=(char *)"F1service";
    theMenuBarSettings.Text[1]=(char *)"F2 info";
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=0;
    theMenuBarSettings.Text[4]=0;
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=ConfirmService;
    theMenuBarSettings.evtFnks[1]=ConfirmF2;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=0;
    theMenuBarSettings.evtFnks[4]=0;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=ConfirmEscape;
    theMenuBarSettings.evtFnks[7]=ConfirmEnter;

    theMenu = new MenuBar(Spalte1_x,Zeile5_y,MZeile_h,(char*)"Quit",	\
			  &this->theMenuBarSettings,parent);

    this->pTSource = this;//EvtTarget Quelle setzen
    this->EvtTargetID=(char*)"ConfirmDialog";
    this->setKeyboardUpEvtHandler(ConfirmDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    theMenu->addToEvtTarget(this);
    this->addEvtTarget(Label_Info);
  }

  void ConfirmDialog::setConfirmMsg(char * Message)
  {
    this->Label_Info->setText(Message);
  }

  static void SplashF11(void * src, SDL_Event * evt)
  {
    //printf("SplashF11\n");
    exit(11);
  }

  static void SplashEnter(void * src, SDL_Event * evt)
  {
    SplashScreen* ad = (SplashScreen*)src;
    //printf("SplashEnter\n");
    ad->Parent->showCalibrationDialog();
  }

  static void SplashF12(void * src, SDL_Event * evt)
  {
    SplashScreen* ad = (SplashScreen*)src;
    //printf("SplashF12\n");
    ad->Parent->showConfirmDialog((char *)"Exit Programm");
  }

  static void SplashScreenKeyListener(void * src, SDL_Event * evt)
  {
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_F11)
	  {
	    SplashF11(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    SplashEnter(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F12)
	  {
	    SplashF12(src,evt);
	  }
	else if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    SplashEnter(src,evt);
	  }
      }
  }

  SplashScreen::~SplashScreen(){}
  SplashScreen::SplashScreen(int sdlw,		\
			     int sdlh,					\
			     int camw,					\
			     int camh,					\
			     int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y;
    short Spalte1_x, Spalte2_x, Spalte3_x;

    short Button_w = 332;
    short x_space = 8;

    this->Parent = parent;
    this->pLogo = 0;
    this->pButtonLogo = 0;

    M_y = sdlh - yPos;

    if(M_y<=84)
      {
	MSpace_h = 2;
	MZeile_h = 18;
      }
    else
      {
	MSpace_h = 5;
	MZeile_h = 28;
      }

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    Spalte1_x = sdlw/2 - 506;
    Spalte2_x = Spalte1_x + 1*Button_w+1*x_space;
    Spalte3_x = Spalte1_x + 2*Button_w+2*x_space;

    //to get rid of compiler warning:
    sprintf(this->Versionsnummer,"%s",CAP_VERSION);
    sprintf(this->Versionsnummer,"%s",FSGPP_VERSION);
    sprintf(this->Versionsnummer,"%s",CAPTURE_VERSION);
    sprintf(this->Versionsnummer,\
	    "PlateExakt V%s © 2012 raantec GmbH & C0. KG, Nienkamp 21, 33829 Borgholzhausen", \
	    CAPCOMPILEDATE);

    Label_Info = new Label(Versionsnummer,\
			   Spalte1_x,\
			   Zeile5_y,\
			   506*2,\
			   MZeile_h,\
			   Parent->DialogSet);

    Label_CrossPosLeft = new Label("---",\
			   Spalte1_x,\
			   yPos+4,\
			   80,\
			   MZeile_h,\
			   Parent->DialogSet);

    Label_CrossPosRight = new Label("---",\
			   Spalte1_x+1012-80,\
			   yPos+4,\
			   80,\
			   MZeile_h,\
			   Parent->DialogSet);

    this->loadImage((char*)"RaLogo.png");
    if(0==pLogo)
      {
	printf("SplashScreen: error loading Logo\n");
      }
    else
      {
	pButtonLogo = new Button(" ",					\
				 (sdlw/2)-(pLogo->w/2),			\
				 yPos+4,				\
				 pLogo->w,				\
				 pLogo->h,				\
				 Parent->MenuSet);
	if(0==pButtonLogo)
	  {
	    printf("SplashScreen: error creating LogoButton\n");
	  }
	else
	  {
	    pButtonLogo->setImages(pLogo,pLogo);
	  }
      }

    this->pTSource = this;//EvtTarget Quelle setzen
    this->EvtTargetID=(char*)"SplashScreen";
    this->setKeyboardUpEvtHandler(SplashScreenKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    this->addEvtTarget(Label_Info);
    this->addEvtTarget(pButtonLogo);
    if(!this->Parent->useTheGUI())
      {
	this->addEvtTarget(Label_CrossPosLeft);
	this->addEvtTarget(Label_CrossPosRight);
      }
    
  }

  int SplashScreen::loadImage(char * path)
  {
    SDL_Surface * tmp = 0;
    //normal
    tmp = IMG_Load(path);
    if(!tmp)
      {
	return -1;
      }
    this->pLogo = SDL_DisplayFormatAlpha(tmp);
    if(!this->pLogo)
      {
	SDL_FreeSurface(tmp);
	return -1;
      }
    SDL_FreeSurface(tmp);
    return 0;
  }

  void SplashScreen::showCrossPosition(int cam,int wert)
  {
    if(!this->Parent->useTheGUI())
      {
	if(0==cam)
	  {
	    snprintf(this->pcCrossPosLeft,128,"%i",wert);
	    Label_CrossPosLeft->setText(this->pcCrossPosLeft);
	  }
	else
	  {
	    snprintf(this->pcCrossPosRight,128,"%i",wert);
	    Label_CrossPosRight->setText(this->pcCrossPosRight);
	  }
      }
  }
}
