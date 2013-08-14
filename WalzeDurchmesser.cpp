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
//#include "Protocol.h"
//#include "pec_cmd.h"
//#include "MBProt.h"

#include "MenuBar.h"
#include "ArbeitsDialog.h"
#include "Dialoge.h"
#include "WalzeDurchmesser.h"

namespace EuMax01
{

  void WalzeDurchmesser::escape_listener(void * src, SDL_Event * evt)
  {
    WalzeDurchmesser* ad = (WalzeDurchmesser*)src;//KeyListener
    ad->Parent->walzeDurchmesserReturn();
  }
 
  void WalzeDurchmesser::return_listener(void * src, SDL_Event * evt)
  {
    WalzeDurchmesser* ad = (WalzeDurchmesser*)src;//KeyListener

    //durchmesser = ad->convertTextToInt();

    ad->Parent->walzeDurchmesserReturn(ad->derDurchmesser);
  }

  void WalzeDurchmesser::WalzeDurchmesserKeyListener(void * src, SDL_Event * evt)
  {
    WalzeDurchmesser* ad = (WalzeDurchmesser*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    //SDLMod mod = key->keysym.mod;
    char zeichen = 0;

    if( key->type == SDL_KEYUP )
      {
	zeichen = Tool::getIntegerNumeric_Char(key);
	if(key->keysym.sym == SDLK_BACKSPACE||zeichen!=0)
	  {
	    //ad->confirmValue(atof(ad->TF_Value->getText()));
	    //ad->getSchritteValues(ad->OldValue,64);
	    //ad->Label_OldValue->setText(ad->OldValue);

	    ad->confirmValue(atof(ad->TF_Value->getText()));
	    ad->getSchritteValues(ad->OldValue,64);
	    ad->Label_OldValue->setText(ad->OldValue);
	  }
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->escape_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->return_listener(src,evt);
	  }
      }
  }

  WalzeDurchmesser::~WalzeDurchmesser(){};
  WalzeDurchmesser::WalzeDurchmesser(int sdlw,				\
				   int sdlh,				\
				   int camw,				\
				   int camh,				\
				   int yPos,NewDialog * parent):Screen(), \
								TF_Len(6)
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y,x1,x2,x3,x_space,button_breit,button_schmal;

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

    button_breit = 400;
    button_schmal = 200;
    x_space = 6;
    x1 = MLinks_x;
    x2 = x1 + button_breit + x_space;
    x3 = x2 + button_schmal + x_space;

    snprintf(this->StepText,256,\
	     "Enter Sleeve diameter :");
    Label_Step = new Label(this->StepText,			\
			   MLinks_x,Zeile1_y,506*2,MZeile_h,Parent->Parent->MenuSet);

    Label_ValueName = new Label(this->ValueName,			\
				x1,Zeile3_y,				\
				button_breit,MZeile_h);  
    Label_ValueName->setText("Sleeve diameter");

    TF_Value = new TextField("",TF_Len,					\
			     x2,					\
			     Zeile3_y,button_schmal,			\
			     MZeile_h,Parent->Parent->WerteSet);
    TF_Value->activateKeyListener(TextField::IntegerNumericChar);
    TF_Value->setBorder(false);
    TF_Value->hide(true);
    TF_Value->setActive(true);

    Label_OldValue = new Label(" -- ",			\
			       x3,			\
			       Zeile3_y,		\
			       button_breit,		\
			       MZeile_h,		\
			       Parent->Parent->WerteSet);

    theMenuBarSettings.Text[0]=0;
    theMenuBarSettings.Text[1]=0;
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=0;
    theMenuBarSettings.Text[4]=0;
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=0;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=0;
    theMenuBarSettings.evtFnks[4]=0;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=escape_listener;
    theMenuBarSettings.evtFnks[7]=return_listener;

    theMenu = new MenuBar((int)MLinks_x,(int)Zeile5_y,(int)MZeile_h,(char*)"Sleeve", \
			  &this->theMenuBarSettings,Parent->Parent);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->EvtTargetID=(char*)"WalzeDurchmesser";
    this->setKeyboardUpEvtHandler(WalzeDurchmesserKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!

    addEvtTarget(Label_Step);
    addEvtTarget(Label_ValueName);
    addEvtTarget(TF_Value);
    addEvtTarget(Label_OldValue);

    theMenu->addToEvtTarget(this);
  }

  void WalzeDurchmesser::useWalzeDurchmesser(int durchmesser)
  {
    derDurchmesser = durchmesser;
    TF_Value->setText((char *)"");//hidden TextField reset
    snprintf(Value,64,"%i",derDurchmesser);
    TF_Value->setText(Value);
    this->showEingabeSchritt();
  }

  void WalzeDurchmesser::confirmValue(int val)
  {
    //printf("confirmValue : %i\n",val);
    derDurchmesser = val;
  }

  void WalzeDurchmesser::showEingabeSchritt()
  {
    this->Label_OldValue->hide(false);
    getSchritteValues(this->OldValue,64);
    this->Label_OldValue->setText(this->OldValue);
    this->Label_OldValue->show(this->Parent->Parent->theGUI->getMainSurface());
    
    //this->Label_Step->show(this->Parent->Parent->theGUI->getMainSurface());
    //this->Label_ValueName->show(this->Parent->Parent->theGUI->getMainSurface());
  }

  void WalzeDurchmesser::getSchritteValues(char * buf,int len)
  {
    snprintf(buf,len,"%7.2f mm",(float)derDurchmesser/100);
  }
}
