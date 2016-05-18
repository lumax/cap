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

  void G_PosDialog::GLiftUpListener(void* src,SDL_Event * evt)
  {
    G_PosDialog* pd = (G_PosDialog*)src;
    pd->moveButtonAction(evt,ExaktG::AxisZ,ExaktG::DirectionUp);
  }

  void G_PosDialog::GLiftDownListener(void* src,SDL_Event * evt)
  {
    G_PosDialog* pd = (G_PosDialog*)src;
    pd->moveButtonAction(evt,ExaktG::AxisZ,ExaktG::DirectionDown);
  }

  void G_PosDialog::GLiftSetSpeedListener(void* src,SDL_Event * evt)
  {
    G_PosDialog* pd = (G_PosDialog*)src;
    pd->incSpeedLevel();
  }

  void G_PosDialog::G_LeftListener(void* src,SDL_Event * evt)
  {
    G_PosDialog* pd = (G_PosDialog*)src;
    int activeAxis = pd->getActiveAxis();
    if(activeAxis==ExaktG::AxisA)
      {
	pd->moveButtonAction(evt,activeAxis,ExaktG::DirectionUp);
	printf("G_PosDialog::G_LeftListener DirectionUp\n");
      }
    else
      {
	pd->moveButtonAction(evt,activeAxis,ExaktG::DirectionLeft);
	printf("G_PosDialog::G_LeftListener DirectionLeft\n");
      }
  }

  void G_PosDialog::G_RightListener(void* src,SDL_Event * evt)
  {
    G_PosDialog* pd = (G_PosDialog*)src;
    int activeAxis = pd->getActiveAxis();
    if(activeAxis==ExaktG::AxisA)
      {
	pd->moveButtonAction(evt,activeAxis,ExaktG::DirectionDown);
	printf("G_PosDialog::G_LeftListener DirectionDowm\n");
      }
    else
      {
	pd->moveButtonAction(evt,activeAxis,ExaktG::DirectionRight);
	printf("G_PosDialog::G_LeftListener DirectionRight\n");
      }
  }

  void G_PosDialog::G_SpeedListener(void* src,SDL_Event * evt)
  {
    G_PosDialog* pd = (G_PosDialog*)src;
    pd->incSpeedLevel();
  }

  void G_PosDialog::G_MoveBtnMouseOverListener(void * src,bool selected)
  {
    G_PosDialog* pd = (G_PosDialog*)src;
    ExaktG * pExaktG = pd->Parent->getExaktG();
    if(!selected){
      pExaktG->holdMoving(false);
    }
  }

  void G_PosDialog::incSpeedLevel(void)
  {
    MenuGBase * gMenu;
    int speedLevel;
    ExaktG * pExaktG = this->Parent->getExaktG();

    gMenu = theMenuG;

    pExaktG->incSpeedLevel(ExaktG::AxisX);
    pExaktG->incSpeedLevel(ExaktG::AxisY);
    pExaktG->incSpeedLevel(ExaktG::AxisA);
    pExaktG->incSpeedLevel(ExaktG::AxisZ);
    speedLevel = pExaktG->getSpeedLevel(ExaktG::AxisX);
    gMenu->pLSpeed->setText(pExaktG->getSpeedText(ExaktG::AxisX		\
							,speedLevel));
    gMenu->pLSpeed->show(Parent->theGUI->getMainSurface());
  }


  void G_PosDialog::moveButtonAction(SDL_Event * evt,int axis,int direction)
  {
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    ExaktG * pExaktG = Parent->getExaktG();

    if(evt->type==SDL_MOUSEBUTTONDOWN)
      {
	pExaktG->move(axis,direction);
	pExaktG->holdMoving(true);
      }
    if((evt->type==SDL_MOUSEBUTTONUP))
      {
	pExaktG->holdMoving(false);
      }
    if(key->type == SDL_KEYUP)
      {
	if((key->keysym.sym == SDLK_LEFT)	\
	   ||(key->keysym.sym == SDLK_RIGHT)	\
	   ||(key->keysym.sym == SDLK_PAGEUP)	\
	   ||(key->keysym.sym == SDLK_PAGEDOWN))
	  {
	    pExaktG->move(axis,direction);
	    pExaktG->holdMoving(false);
	  }
      }
  }
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

  int G_PosDialog::getActiveAxis(void)
  {
    return this->activeAxis;
  }

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
    this->Parent = parent;
    this->activeAxis = ExaktG::AxisX;

    unsigned short MSpace_h, MZeile_h;
    short M_y = sdlh - yPos;
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

    //short MLinks_x = sdlw/2 - 504;//Breite von 1008 und mittig

    //vertikal fünf Zeilen
    short Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    //Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    //short Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    //short Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    //short Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    //horizontal acht Spalten
    int width = 1008;
    int x_space = 2;
    int Bw = (width -7*x_space)/8;
    int B1x = sdlw/2 - width/2;
    //int B2x = B1x + 1*Bw+1*x_space;
    //int B3x = B1x + 2*Bw+2*x_space;
    int B4x = B1x + 3*Bw+3*x_space;
    //int B5x = B1x + 4*Bw+4*x_space;
    int B6x = B1x + 5*Bw+5*x_space;
    int B7x = B1x + 6*Bw+6*x_space;
    int B8x = B1x + 7*Bw+7*x_space;

    this->LabelActual->setText((char*)"Position");
    LabelActual->setWidth(Bw);
    pLabelCam1[PosDialog::iCurr]->setWidth(Bw);
    pLabelCam2[PosDialog::iCurr]->setWidth(Bw);
    pLabelZ[PosDialog::iCurr]->setWidth(Bw);

    pLabelCam1[PosDialog::iDiff]->setPosX(B4x+x_space);
    pLabelCam2[PosDialog::iDiff]->setPosX(B4x+x_space);
    pLabelZ[PosDialog::iDiff]->setPosX(B4x+x_space);

    LabelStep->setPosX(B6x+x_space);
    pLabelCam1[PosDialog::iStep]->setPosX(B6x+x_space);
    pLabelCam2[PosDialog::iStep]->setPosX(B6x+x_space);
    pLabelZ[PosDialog::iStep]->setPosX(B6x+x_space);

    LabelStep->setWidth(Bw);
    pLabelCam1[PosDialog::iStep]->setWidth(Bw);
    pLabelCam2[PosDialog::iStep]->setWidth(Bw);
    pLabelZ[PosDialog::iStep]->setWidth(Bw);

    struct t_MenuGSettings * pGMset;

    pGMset = &theMenuGSettings;
    pGMset->evtSource = this;
    pGMset->evtFnkBtn1Up = G_LeftListener;
    pGMset->evtFnkBtn1Down = G_LeftListener;
    pGMset->evtFnkBtn1MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkBtn2MouseOver = G_MoveBtnMouseOverListener;
    pGMset->evtFnkBtn2Up = G_RightListener;
    pGMset->evtFnkBtn2Down = G_RightListener;
    pGMset->evtFnkSetSpeed = G_SpeedListener;
    pGMset->btn1Text = (char *)"<-";
    pGMset->btn2Text = (char *)"->";
    pGMset->SpeedLabelText = (char *)"xxx";//pExaktG->getSpeedText(ExaktG::AxisX,0);

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
    pGMset->SpeedLabelText = (char*)("zzz");//pExaktG->getSpeedText(ExaktG::AxisZ,0);

    theMenuG = new MenuGVertical((char *)"X-Achse",			\
				 B7x+x_space,				\
				 (int)Zeile1_y,				\
				 MZeile_h,				\
				 MSpace_h,				\
				 Bw,					\
				 &this->theMenuGSettings,Parent);

    theMenuG->addToEvtTarget(Parent);

    theMenuGZ = new MenuGVertical((char *)"Lift",			\
				  B8x+x_space,				\
				 (int)Zeile1_y,				\
				 MZeile_h,				\
				 MSpace_h,				\
				 Bw,				\
				 &this->theMenuGZSettings,Parent);

    theMenuGZ->addToEvtTarget(Parent);

    /*


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
