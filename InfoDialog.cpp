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

#include <linux/videodev.h>
#include "V4L2_utils.h"
#include "CamCtrl.h"

namespace EuMax01
{

  static void InfoDialogKeyListener(void * src, SDL_Event * evt)
  {
    InfoDialog* ad = (InfoDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    ad->CamCtrlContainer[ad->aktCamCtrl]->keyEventOccured(key);

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->Parent->showArbeitsDialog();
	  }
	else if(key->keysym.sym == SDLK_F1)
	  {
	    ad->refreshAll();
	  }
	else if(key->keysym.sym == SDLK_RETURN)
	  {
	    //exit(12);
	  }
	else if(key->keysym.sym == SDLK_LEFT)
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
	  }
      }
  }

  InfoDialog::InfoDialog(int sdlw,		\
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
    this->aktCamCtrl = 0;

    for(int i = 0;i < InfoDialog::CamCtrlContLen; i++)
      {
	CamCtrlContainer[i] = 0;
      }

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

    /*
      Control Brightness
      Control Contrast
      Control Saturation
      Control Hue
      Control White Balance Temperature, Auto
      Control Gamma
      Control Gain
      Control Power Line Frequency
      Menu items:
      Disabled
      50 Hz
      60 Hz
      Control White Balance Temperature
      Control Sharpness
      Control Backlight Compensation
     */
    CamCtrlContainer[0] = new CamCtrl((int)Spalte1_x,			\
				      (int)Zeile1_y,			\
				      (int)MZeile_h,			\
				      0,				\
				      (char*)"Gain",		\
				      V4L2_CID_GAIN,		\
				      parent);
    
    CamCtrlContainer[1] = new CamCtrl((int)sdlw/2+6,			\
				      (int)Zeile1_y,			\
				      (int)MZeile_h,			\
				      0,				\
				      (char*)"Sharpness",		\
				      V4L2_CID_SHARPNESS,		\
				      parent);
    
    CamCtrlContainer[2] = new CamCtrl((int)Spalte1_x,			\
				      (int)Zeile2_y,			\
				      (int)MZeile_h,			\
				      0,				\
				      (char*)"Brightness",		\
				      V4L2_CID_BRIGHTNESS,		\
				      parent);
    
    CamCtrlContainer[3] = new CamCtrl((int)sdlw/2+6,			\
				      (int)Zeile2_y,			\
				      (int)MZeile_h,			\
				      0,				\
				      (char*)"Contrast",		\
				      V4L2_CID_CONTRAST,		\
				      parent);

    CamCtrlContainer[4] = new CamCtrl((int)Spalte1_x,	\
				      (int)Zeile3_y,	\
				      (int)MZeile_h,	\
				      0,		\
				      (char*)"Hue",	\
				      V4L2_CID_HUE,	\
				      parent);

    CamCtrlContainer[5] = new CamCtrl((int)sdlw/2+6,	\
				      (int)Zeile3_y,	\
				      (int)MZeile_h,	\
				      0,		\
				      (char*)"Saturation",	\
				      V4L2_CID_SATURATION,	\
				      parent);

    CamCtrlContainer[6] = new CamCtrl((int)Spalte1_x,		\
				      (int)Zeile4_y,		\
				      (int)MZeile_h,		\
				      0,			\
				      (char*)"Gamma",	\
				      V4L2_CID_GAMMA,	\
				      parent);

    CamCtrlContainer[7] = new CamCtrl((int)sdlw/2+6,			\
				      (int)Zeile4_y,			\
				      (int)MZeile_h,			\
				      0,				\
				      (char*)"Backlight Compensation",	\
				      V4L2_CID_BACKLIGHT_COMPENSATION,	\
				      parent);

    Label_MenuTitle = new Label("Info",Spalte1_x,Zeile5_y,150,MZeile_h,Parent->MenuSet);

    Label_Menu = new Label(" ",		\
			   Spalte1_x+158,Zeile5_y,1012-158,MZeile_h,Parent->MenuSet);

    /*Label_Info = new Label("INFO",		\
			   Spalte1_x,\
			   Zeile1_y,\
			   506*2,\
			   MZeile_h,\
			   Parent->DialogSet);*/

    refreshAll();

    this->pTSource = this;//EvtTarget Quelle setzen
    this->setKeyboardUpEvtHandler(InfoDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    this->addEvtTarget(Label_MenuTitle);
    this->addEvtTarget(Label_Menu);
    //this->addEvtTarget(Label_Info);
    for(int i = 0;i < InfoDialog::CamCtrlContLen; i++)
      {
	if(CamCtrlContainer[i]!=0)
	  {
	    CamCtrlContainer[i]->addToEvtTarget(this);
	  }
      }
  }

  void InfoDialog::refreshAll()
  {
     for(int i = 0;i < InfoDialog::CamCtrlContLen; i++)
      {
	if(CamCtrlContainer[i]!=0)
	  {
	    CamCtrlContainer[i]->setFocus(false);
	    CamCtrlContainer[i]->refreshValues();	    
	  }
      }
     if(CamCtrlContainer[aktCamCtrl]!=0)
       CamCtrlContainer[aktCamCtrl]->setFocus(true);
  }

  /*
    0  |  1
    2  |  3
    4  |  5
   */

  void InfoDialog::left()
  {
    int tmp = 0;
    if(aktCamCtrl%2==1)//ungerade gerade
      {
	tmp = aktCamCtrl-1;
	if(tmp>=0&&tmp<=InfoDialog::CamCtrlContLen-1)
	  {
	    if(CamCtrlContainer[tmp]!=0)
	      {
		CamCtrlContainer[aktCamCtrl]->setFocus(false);
		aktCamCtrl = tmp;
		CamCtrlContainer[aktCamCtrl]->setFocus(true);
		CamCtrlContainer[aktCamCtrl]->refreshValues();	
	      }
	  }
      }
  }

  void InfoDialog::right()
  {
    int tmp = 0;
    if(aktCamCtrl%2==0)//ungerade gerade
      {
	tmp = aktCamCtrl+1;
	if(tmp>=0&&tmp<=InfoDialog::CamCtrlContLen-1)
	  {
	    if(CamCtrlContainer[tmp]!=0)
	      {
		CamCtrlContainer[aktCamCtrl]->setFocus(false);
		aktCamCtrl = tmp;
		CamCtrlContainer[aktCamCtrl]->setFocus(true);
		CamCtrlContainer[aktCamCtrl]->refreshValues();	
	      }
	  }
      }
  }

  void InfoDialog::up()
  {
    int tmp = 0;
    if(aktCamCtrl>1)
      {
	tmp = aktCamCtrl - 2;
	if(tmp>=0&&tmp<=InfoDialog::CamCtrlContLen-1)
	  {
	    if(CamCtrlContainer[tmp]!=0)
	      {
		CamCtrlContainer[aktCamCtrl]->setFocus(false);
		aktCamCtrl = tmp;
		CamCtrlContainer[aktCamCtrl]->setFocus(true);
		CamCtrlContainer[aktCamCtrl]->refreshValues();	
	      }
	  }
      }
  }

  void InfoDialog::down()
  {
    int tmp = 0;
    if(aktCamCtrl<InfoDialog::CamCtrlContLen-2)
      {
	tmp = aktCamCtrl + 2;
		if(tmp>=0&&tmp<=InfoDialog::CamCtrlContLen-1)
	  {
	    if(CamCtrlContainer[tmp]!=0)
	      {
		CamCtrlContainer[aktCamCtrl]->setFocus(false);
		aktCamCtrl = tmp;
		CamCtrlContainer[aktCamCtrl]->setFocus(true);
		CamCtrlContainer[aktCamCtrl]->refreshValues();	
	      }
	  }
      }
  }
}
