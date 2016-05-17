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
#include "g_ctrl/G_Ctrl.h"
#include "g_ctrl/StreamScanner.h"
#include "g_ctrl/ExaktG.h"
#include "Dialoge.h"

#include "G_PosDialog.h"

namespace EuMax01
{
  /*  void G_TestDialog::GX_LeftListener(void* src,SDL_Event * evt)
  {
    G_TestDialog* td = (G_TestDialog*)src;
    td->moveButtonAction(evt,ExaktG::AxisX,ExaktG::DirectionLeft);
  }

  void G_TestDialog::GX_RightListener(void* src,SDL_Event * evt)
  {
     G_TestDialog* td = (G_TestDialog*)src;
     td->moveButtonAction(evt,ExaktG::AxisX,ExaktG::DirectionRight);
  }

  void G_TestDialog::GX_SpeedListener(void* src,SDL_Event * evt)
  {
    G_TestDialog* td = (G_TestDialog*)src;
    td->incSpeedLevel(ExaktG::AxisX);
  }

  void G_TestDialog::G_MoveBtnMouseOverListener(void * src,bool selected)
  {
    G_TestDialog* td = (G_TestDialog*)src;
    if(!selected){
      td->pExaktG->holdMoving(false);
    }
  }
  */

  /*  static void G_TestDialogKeyListener(void * src, SDL_Event * evt)
  {
    G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    //    SDLMod mod = key->keysym.mod;

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
	else if(key->keysym.sym == SDLK_F4)
	  {

	  }
	else if(key->keysym.sym == SDLK_F5)
	  {
	    ad->getstatus_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEUP)
	  {
	    ad->GLiftUpListener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEDOWN)
	  {
	    ad->GLiftDownListener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_LEFT)
	  {
	    ad->GY_LeftListener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RIGHT)
	  {
	    ad->GY_RightListener(src,evt);
	  }
      }
  }*/

  /*  void G_TestDialog::moveButtonAction(SDL_Event * evt,int axis,int direction)
  {
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if(evt->type==SDL_MOUSEBUTTONDOWN)
      {
	this->pExaktG->move(axis,direction);
	this->pExaktG->holdMoving(true);
      }
    if((evt->type==SDL_MOUSEBUTTONUP))
      {
	this->pExaktG->holdMoving(false);
      }
    if(key->type == SDL_KEYUP)
      {
	if((key->keysym.sym == SDLK_LEFT)	\
	   ||(key->keysym.sym == SDLK_RIGHT)	\
	   ||(key->keysym.sym == SDLK_PAGEUP)	\
	   ||(key->keysym.sym == SDLK_PAGEDOWN))
	  {
	    this->pExaktG->move(axis,direction);
	    this->pExaktG->holdMoving(false);
	  }
      }
      }*/

  /*  void G_TestDialog::incSpeedLevel(int axis)
  {
    MenuGBase * gMenu;
    int speedLevel;

    if(ExaktG::AxisX==axis){
      gMenu = theMenuGX;
    }
    else if(ExaktG::AxisY==axis){
      gMenu = theMenuGY;
    }
    else if(ExaktG::AxisZ==axis){
      gMenu = theMenuGZ;
    }
    else if(ExaktG::AxisA==axis){
      gMenu = theMenuGA;
    }
    this->pExaktG->incSpeedLevel(axis);
    speedLevel = this->pExaktG->getSpeedLevel(axis);
    gMenu->pLSpeed->setText(this->pExaktG->getSpeedText(axis,speedLevel));
    gMenu->pLSpeed->show(Parent->theGUI->getMainSurface());
    }*/

  G_PosDialog::G_PosDialog(char* text,		\
		       int sdlw,		\
		       int sdlh,		\
		       int camw,		\
		       int camh,		\
		       int yPos,\
			   ArbeitsDialog * parent):PosDialog(text,\
							     sdlw,\
							     sdlh,\
							     camw,\
							     camw,\
							     yPos,parent)
  {

    this->LabelActual->setText((char*)"GCurrent Position");
    /*    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    short Zeile1_y;
    //short Zeile3_y;
    short Zeile5_y;

    this->Parent = parent;
    this->pExaktG = this->Parent->getExaktG();
    this->pGCtrl = this->pExaktG->getG_Ctrl();

    this->tGCodeLis.pTheListener = this;
    this->tGCodeLis.fnkXPosLis =&this->xPosLis;
    this->tGCodeLis.fnkYPosLis =&this->yPosLis;
    this->tGCodeLis.fnkZPosLis =&this->zPosLis;
    this->tGCodeLis.fnkAPosLis =&this->aPosLis;
    this->tGCodeLis.fnkGFLis =&this->gFLis;

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

    //vertikal fünf Zeilen
    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    //Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    //short Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    short Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    //horizontal acht Spalten
    int width = 1008;
    int x_space = 2;
    int Bw = (width -7*x_space)/8;
    int B1x = sdlw/2 - width/2;
    int B2x = B1x + 1*Bw+1*x_space;
    int B3x = B1x + 2*Bw+2*x_space;
    //int B4x = B1x + 3*Bw+3*x_space;
    int B5x = B1x + 4*Bw+4*x_space;
    int B6x = B1x + 5*Bw+5*x_space;
    int B7x = B1x + 6*Bw+6*x_space;
    //int B8x = B1x + 7*Bw+7*x_space;

    theMenuBarSettings.Text[0]=0;
    theMenuBarSettings.Text[1]=0;
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=0;
    theMenuBarSettings.Text[4]=(char *)"F5 status";
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=0;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=0;
    theMenuBarSettings.evtFnks[4]=getstatus_listener;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=escape_listener;
    theMenuBarSettings.evtFnks[7]=return_listener;

    theMenu = new MenuBar((int)MLinks_x,(int)Zeile5_y,(int)MZeile_h,(char*)"G-Test", \
			  &this->theMenuBarSettings,Parent);

    struct t_MenuGSettings * pGMset;

    pGMset = &theMenuGXSettings;
    pGMset->evtSource = this;
    pGMset->evtFnkBtn1Up = GX_LeftListener;
    pGMset->evtFnkBtn1Down = GX_LeftListener;
    pGMset->evtFnkBtn1MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkBtn2MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkBtn2Up = GX_RightListener;
    pGMset->evtFnkBtn2Down = GX_RightListener;
    pGMset->evtFnkSetSpeed = GX_SpeedListener;
    pGMset->btn1Text = (char *)"<--a";
    pGMset->btn2Text = (char *)"d-->";
    pGMset->SpeedLabelText = pExaktG->getSpeedText(ExaktG::AxisX,0);
    
    pGMset = &theMenuGYSettings;
    pGMset->evtSource = this;
    pGMset->evtFnkBtn1Up = GY_LeftListener;
    pGMset->evtFnkBtn1Down = GY_LeftListener;
    pGMset->evtFnkBtn2Up = GY_RightListener;
    pGMset->evtFnkBtn2Down = GY_RightListener;
    pGMset->evtFnkBtn1MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkBtn2MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkSetSpeed = GY_SpeedListener;
    pGMset->btn1Text = (char *)"Left";
    pGMset->btn2Text = (char *)"Right";
    pGMset->SpeedLabelText = pExaktG->getSpeedText(ExaktG::AxisY,0);

    pGMset = &theMenuGASettings;
    pGMset->evtSource = this;
    pGMset->evtFnkBtn1Up = GA_UpListener;
    pGMset->evtFnkBtn1Down = GA_UpListener;
    pGMset->evtFnkBtn2Up = GA_DownListener;
    pGMset->evtFnkBtn2Down = GA_DownListener;
    pGMset->evtFnkBtn1MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkBtn2MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkSetSpeed = GA_SpeedListener;
    pGMset->btn1Text = (char *)"Up";
    pGMset->btn2Text = (char *)"Down";
    pGMset->SpeedLabelText = pExaktG->getSpeedText(ExaktG::AxisA,0);

    pGMset = &theMenuGZSettings;
    pGMset->evtSource = this;
    pGMset->evtFnkBtn1Up = GLiftUpListener;
    pGMset->evtFnkBtn1Down = GLiftUpListener;
    pGMset->evtFnkBtn2Up = GLiftDownListener;
    pGMset->evtFnkBtn2Down = GLiftDownListener;
    pGMset->evtFnkBtn1MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkBtn2MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkSetSpeed = GLiftSetSpeedListener;
    pGMset->btn1Text = (char *)"PgUp";
    pGMset->btn2Text = (char *)"PgDo";
    pGMset->SpeedLabelText = pExaktG->getSpeedText(ExaktG::AxisZ,0);

    theMenuGX = new MenuGVertical((char *)"X-Achse",			\
				    B1x,				\
				    (int)Zeile1_y,			\
				    MZeile_h,				\
				    MSpace_h,				\
				    2*Bw,				\
				    &this->theMenuGXSettings,Parent);

    theMenuGY = new MenuGVertical((char *)"Y-Achse",			\
				    B3x,				\
				    (int)Zeile1_y,			\
				    MZeile_h,				\
				    MSpace_h,				\
				    2*Bw,				\
				    &this->theMenuGYSettings,Parent);

    theMenuGA = new MenuGVertical((char *)"Walze",		\
				 B5x,					\
				 (int)Zeile1_y,				\
				 MZeile_h,				\
				 MSpace_h,				\
				 2*Bw,				\
				 &this->theMenuGASettings,Parent);

    theMenuGZ = new MenuGVertical((char *)"Lift",			\
				 B7x,					\
				 (int)Zeile1_y,				\
				 MZeile_h,				\
				 MSpace_h,				\
				 2*Bw,				\
				 &this->theMenuGZSettings,Parent);

    pBEingabe = new Button("Enter",					\
			   B1x,						\
			   (int)Zeile4_y,				\
			   Bw,					\
			   MZeile_h,					\
			   Parent->MenuSet);
    pBEingabe->setLMButtonUpEvtHandler(return_listener);
    pBEingabe->pTSource = this;

    pTFEingabe = new TextField(0,			\
			       64,				\
			       B2x,					\
			       (int)Zeile4_y,				\
			       3*Bw,					\
			       MZeile_h,				\
			       Parent->WerteSet);
    pTFEingabe->setActive(true);

    pBZeroX = new Button("ZeroX",					\
			B5x,						\
			(int)Zeile4_y,					\
			Bw,						\
			MZeile_h,					\
			Parent->MenuSet);
    pBZeroX->setLMButtonUpEvtHandler(zeroX_listener);
    pBZeroX->pTSource = this;

    pBZeroY = new Button("ZeroY",					\
			B6x,						\
			(int)Zeile4_y,					\
			Bw,						\
			MZeile_h,					\
			Parent->MenuSet);
    pBZeroY->setLMButtonUpEvtHandler(zeroY_listener);
    pBZeroY->pTSource = this;

    pBZeroA = new Button("ZeroA",					\
			B7x,						\
			(int)Zeile4_y,					\
			Bw,						\
			MZeile_h,					\
			Parent->MenuSet);
    pBZeroA->setLMButtonUpEvtHandler(zeroA_listener);
    pBZeroA->pTSource = this;

    //addEvtTarget(Label_Value);
    theMenu->addToEvtTarget(this);

    theMenuGX->addToEvtTarget(this);
    theMenuGY->addToEvtTarget(this);
    theMenuGA->addToEvtTarget(this);
    theMenuGZ->addToEvtTarget(this);
    this->addEvtTarget(pBEingabe);
    this->addEvtTarget(pTFEingabe);
    this->addEvtTarget(pBZeroX);
    this->addEvtTarget(pBZeroY);
    this->addEvtTarget(pBZeroA);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(G_TestDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
*/
  }

  /*void G_PosDialog::setActive(void)
  {
    this->pExaktG->setGCodeResultListener(&this->tGCodeLis);
    this->pGCtrl->cmdGetStatus();
  }*/
}/* end Namespace */
