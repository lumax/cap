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
#include "InfoDialog.h"
#include "GMenu.h"

//#include "videodev.h"
//#include "V4L2_utils.h"
//#include "CamCtrl.h"
#include "G_TestDialog.h"

namespace EuMax01
{

  void G_TestDialog::GLiftUpListener(void*,SDL_Event * evt)
  {
    printf("G_TestDialog MenuGLiftUpListener\n");
  }

  void G_TestDialog::GLiftDownListener(void*,SDL_Event * evt)
  {
    printf("G_TestDialog MenuGLiftDownListener\n");
  }

  void G_TestDialog::GLiftSetSpeedListener(void*,SDL_Event * evt)
  {
    printf("G_TestDialog MenuGLiftSetSpeedListener\n");
  }

  void G_TestDialog::escape_listener(void * src, SDL_Event * evt)
  {
    G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
    ad->Parent->showArbeitsDialog();
  }

  void G_TestDialog::left_listener(void * src, SDL_Event * evt)
  {
    //G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
    printf("G_TestDialog leftListener\n");
  }

  void G_TestDialog::right_listener(void * src, SDL_Event * evt)
  {
    //G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
     printf("G_TestDialog rightListener\n");
  }
  
  void G_TestDialog::return_listener(void * src, SDL_Event * evt)
  {
    //G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
    printf("G_TestDialog returnListener\n");
  }

  static void G_TestDialogKeyListener(void * src, SDL_Event * evt)
  {
    G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    SDLMod mod = key->keysym.mod;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->escape_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->return_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_LEFT || key->keysym.sym == SDLK_F4)
	  {
	    ad->left_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RIGHT || key->keysym.sym == SDLK_F5)
	  {
	    ad->right_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEUP)
	  {
	    ad->GLiftUpListener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEUP)
	  {
	    ad->GLiftDownListener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_s)
	  {
	    ad->GLiftSetSpeedListener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_p)
	  {
	    if((mod & KMOD_CTRL) &&		\
	       (mod & KMOD_SHIFT)&&             \
	       (mod & KMOD_ALT))
	      {
		printf("G_TestDialog Str Shift Alt p\n");
	      }
	  }
      }
  }

  G_TestDialog::G_TestDialog(int sdlw,		\
			 int sdlh,		\
			 int camw,		\
			 int camh,		\
			 int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile3_y,Zeile5_y;
    int wGLift;
    int xGLift;
    //int maxWidth;

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

    MLinks_x = sdlw/2 - 504;//Breite von 1008 und mittig
    wGLift = 126;//etwa ein Achtel der Gesamten Breite
    xGLift = MLinks_x + 504*2 - wGLift;
    //maxWidth = 504*2 - wGLift - MSpace_h;

    //vertikal fünf Zeilen
    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    //Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    //Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    //horizontal acht Spalten
    int width = 1008;
    int x_space = 3;
    int Bw = (width -7*x_space)/8;
    int B1x = sdlw/2 - width/2;
    //int B2x = B1x + 1*Bw+1*x_space;
    //int B3x = B1x + 2*Bw+2*x_space;
    int B4x = B1x + 3*Bw+3*x_space;
    int B5x = B1x + 4*Bw+4*x_space;
    //int B6x = B1x + 5*Bw+5*x_space;
    //int B7x = B1x + 6*Bw+6*x_space;
    int B8x = B1x + 7*Bw+7*x_space;


    snprintf(this->StepText,256,\
	     "Move camera 1 X-Axis in zero position :");
    Label_Step = new Label(this->StepText,			\
			   B1x,Zeile1_y,7*Bw+x_space,MZeile_h,Parent->MenuSet);
    Label_Step->setFontColor(&Globals::GlobalSDL_Color3);//ein rot

    Label_ValueName = new Label(this->ValueName,			\
				B1x,Zeile3_y,			\
				4*Bw,MZeile_h);
    Label_ValueName->setText(Parent->TextCam1Xaxis);

    snprintf(this->Value,64,"---");
    Label_Value = new Label(Value,					\
			    B5x,					\
			    Zeile3_y,3*Bw,			\
			    MZeile_h,Parent->WerteSet);
    Label_Value->setBorder(true);

    theMenuBarSettings.Text[0]=0;
    theMenuBarSettings.Text[1]=0;
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=(char *)"F4 prev";
    theMenuBarSettings.Text[4]=(char *)"F5 next";
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=0;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=left_listener;
    theMenuBarSettings.evtFnks[4]=right_listener;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=escape_listener;
    theMenuBarSettings.evtFnks[7]=return_listener;

    theMenu = new MenuBar((int)MLinks_x,(int)Zeile5_y,(int)MZeile_h,(char*)"G-Test", \
			  &this->theMenuBarSettings,Parent);

    theMenuGLiftSettings.evtSource = (void*)this;
    theMenuGLiftSettings.evtFnkUp = GLiftUpListener;
    theMenuGLiftSettings.evtFnkDown = GLiftDownListener;
    theMenuGLiftSettings.evtFnkSetSpeed = GLiftSetSpeedListener;

    theMenuGLift = new MenuGLift(B8x,				\
				 (int)Zeile1_y,				\
				 MZeile_h,				\
				 MSpace_h,				\
				 Bw,				\
				 &this->theMenuGLiftSettings,Parent);


    addEvtTarget(Label_Step);
    addEvtTarget(Label_ValueName);
    addEvtTarget(Label_Value);
    theMenu->addToEvtTarget(this);

    theMenuGLift->addToEvtTarget(this);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(G_TestDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
  }

}
