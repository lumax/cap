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
	    ad->Parent->showArbeitsDialog();
	  }
	else if(key->keysym.sym == SDLK_RETURN)
	  {
	    ad->Parent->showArbeitsDialog();
	  }
      }
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
    //short MLoadName_y;

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

    //MLoadName_y  = yPos + 1*MSpace_h + 0*MZeile_h;

    Label_Error = new Label("Error",					\
			    MLinks_x,					\
			    yPos + 1*MSpace_h + 0*MZeile_h,		\
			    506*2,MZeile_h);
    
    Label_Info = new Label("----",					\
			   MLinks_x,					\
			   yPos + 2*MSpace_h + 1*MZeile_h,		\
			   506*2,MZeile_h);
    
    Label_OK = new Label("OK  (Enter)",					\
			 MLinks_x,					\
			 yPos + 4*MSpace_h + 3*MZeile_h,		\
			 506,MZeile_h);
    
    this->pTSource = this;//EvtTarget Quelle setzen
    this->EvtTargetID=(char*)"ErrorDialog";
    this->setKeyboardUpEvtHandler(ErrorDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    this->addEvtTarget(Label_Error);
    this->addEvtTarget(Label_Info);
    this->addEvtTarget(Label_OK);
  }

  void ErrorDialog::setErrorMsg(char * Message)
  {
    this->Label_Info->setText(Message);
  }

  static void ConfirmDialogKeyListener(void * src, SDL_Event * evt)
  {
    ConfirmDialog* ad = (ConfirmDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->Parent->showArbeitsDialog();
	  }
	else if(key->keysym.sym == SDLK_RETURN)
	  {
	    exit(12);
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

    Label_MenuTitle = new Label("QUIT",Spalte1_x,Zeile5_y,150,MZeile_h,Parent->MenuSet);

    Label_Menu = new Label("ESC : cancel | ENTER : exit programm",		\
			   Spalte1_x+158,Zeile5_y,1012-158,MZeile_h,Parent->MenuSet);

    Label_Info = new Label("---",\
			   Spalte1_x,\
			   Zeile1_y,\
			   506*2,\
			   MZeile_h,\
			   Parent->DialogSet);

    this->pTSource = this;//EvtTarget Quelle setzen
    this->EvtTargetID=(char*)"ConfirmDialog";
    this->setKeyboardUpEvtHandler(ConfirmDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    this->addEvtTarget(Label_MenuTitle);
    this->addEvtTarget(Label_Menu);
    this->addEvtTarget(Label_Info);
  }

  void ConfirmDialog::setConfirmMsg(char * Message)
  {
    this->Label_Info->setText(Message);
  }
}
