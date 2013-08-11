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
//#include "NewDirectDialog.h"
#include "FadenkreuzDialog.h"

//#include <linux/videodev.h>
//#include "V4L2_utils.h"
//#include "CamCtrl.h"

namespace EuMax01
{

  static void FKAction1(void * src, SDL_Event * evt)
  {
    int tmp;
    FadenkreuzDialog* ad = (FadenkreuzDialog*)src;
    int step = 1;
    SDLMod mod = SDL_GetModState();

    if((mod & KMOD_RSHIFT) || (mod & KMOD_LSHIFT))
      step=2;
    tmp = ad->Parent->getCrossairWidth();
    ad->Parent->setCrossairWidth(tmp-step);
  }

  static void FKAction2(void * src, SDL_Event * evt)
  {
    int tmp;
    FadenkreuzDialog* ad = (FadenkreuzDialog*)src;
    int step = 1;
    SDLMod mod = SDL_GetModState();

    if((mod & KMOD_RSHIFT) || (mod & KMOD_LSHIFT))
      step=2;
    tmp = ad->Parent->getCrossairWidth();
    ad->Parent->setCrossairWidth(tmp+step);
  }

  static void FKActionESC(void * src, SDL_Event * evt)
  {
    FadenkreuzDialog* ad = (FadenkreuzDialog*)src;
    ad->Parent->showArbeitsDialog();
  }

  static void FKActionReturn(void * src, SDL_Event * evt)
  {
    FadenkreuzDialog* ad = (FadenkreuzDialog*)src;
    ad->Parent->saveCrossairWidth();
    ad->Parent->showArbeitsDialog();
  }

  static void FKDialogKeyListener(void * src, SDL_Event * evt)
  {
    FadenkreuzDialog* ad = (FadenkreuzDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    char zeichen = 0;

    if( key->type == SDL_KEYUP )
      {
	zeichen = Tool::getIntegerNumeric_Char(key);
	if(key->keysym.sym == SDLK_BACKSPACE||zeichen!=0)
	  {
	    printf("ad->TF_Value->getText():%s\n",ad->TF_Value->getText());
	    //ad->confirmValue(atof(ad->TF_Value->getText()));
	    //ad->getSchritteValues(ad->OldValue,64);
	    //ad->Label_OldValue->setText(ad->OldValue);
	  }
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    FKActionESC(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F4 || key->keysym.sym == SDLK_LEFT )
	  {
	    FKAction1(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F5 || key->keysym.sym == SDLK_RIGHT )
	  {
	    FKAction2(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    FKActionReturn(src,evt);
	  }
	/*	else if(key->keysym.sym == SDLK_LEFT)
	  {
	    ad->left();
	  }
	else if(key->keysym.sym == SDLK_RIGHT)
	  {
	    ad->right();
	  }
	else if(key->keysym.sym == SDLK_UP)
	  {
	    ad->up();
	  }
	else if(key->keysym.sym == SDLK_DOWN)
	  {
	    ad->down();
	    }*/
      }
  }

  FadenkreuzDialog::FadenkreuzDialog(int sdlw,				\
				     int sdlh,				\
				     int camw,				\
				     int camh,				\
				     int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y;
    short Spalte1_x, Spalte2_x, Spalte3_x, x1, x2, x3, button_breit,button_schmal;

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

    MLinks_x = sdlw/2 - 506;

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    Spalte1_x = sdlw/2 - 506;
    Spalte2_x = Spalte1_x + 1*Button_w+1*x_space;
    Spalte3_x = Spalte1_x + 2*Button_w+2*x_space;

    button_breit = 400;
    button_schmal = 200;
    x_space = 6;
    x1 = MLinks_x;
    x2 = x1 + button_breit + x_space;
    x3 = x2 + button_schmal + x_space;

    snprintf(this->StepText,256,\
	     "Adjust crosshair width:");
    Label_Step = new Label(this->StepText,			\
			   MLinks_x,Zeile1_y,506*2,MZeile_h,Parent->MenuSet);

    theMenuBarSettings.Text[0]=0;
    theMenuBarSettings.Text[1]=0;
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=(char *)"F4 -";
    theMenuBarSettings.Text[4]=(char *)"F5 +";
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=0;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=FKAction1;
    theMenuBarSettings.evtFnks[4]=FKAction2;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=FKActionESC;
    theMenuBarSettings.evtFnks[7]=FKActionReturn;

    theMenu = new MenuBar(Spalte1_x,Zeile5_y,MZeile_h,(char*)"Cross width",	\
			  &this->theMenuBarSettings,parent);

    this->pTSource = this;//EvtTarget Quelle setzen
    this->EvtTargetID=(char*)"FKDialog";
    this->setKeyboardUpEvtHandler(FKDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    theMenu->addToEvtTarget(this);

    addEvtTarget(Label_Step);
  }
}
