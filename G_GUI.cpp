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

#include "G_GUI.h"

namespace EuMax01
{
  void G_GUI::GLiftUpListener(void* src,SDL_Event * evt)
  {
    G_GUI* pd = (G_GUI*)src;
    pd->moveButtonAction(evt,ExaktG::AxisZ,ExaktG::DirectionUp);
  }

  void G_GUI::GLiftDownListener(void* src,SDL_Event * evt)
  {
    G_GUI* pd = (G_GUI*)src;
    pd->moveButtonAction(evt,ExaktG::AxisZ,ExaktG::DirectionDown);
  }

  void G_GUI::GLiftSetSpeedListener(void* src,SDL_Event * evt)
  {
    G_GUI* pd = (G_GUI*)src;
    pd->incSpeedLevel();
  }

  void G_GUI::G_LeftListener(void* src,SDL_Event * evt)
  {
    G_GUI* pd = (G_GUI*)src;
    int activeAxis = pd->getActiveAxis();
    if(activeAxis==ExaktG::AxisA)
      {
	pd->moveButtonAction(evt,activeAxis,ExaktG::DirectionUp);
	printf("G_GUI::G_LeftListener DirectionUp\n");
      }
    else
      {
	pd->moveButtonAction(evt,activeAxis,ExaktG::DirectionLeft);
	printf("G_GUI::G_LeftListener DirectionLeft\n");
      }
  }

  void G_GUI::G_RightListener(void* src,SDL_Event * evt)
  {
    G_GUI* pd = (G_GUI*)src;
    int activeAxis = pd->getActiveAxis();
    if(activeAxis==ExaktG::AxisA)
      {
	pd->moveButtonAction(evt,activeAxis,ExaktG::DirectionDown);
	printf("G_GUI::G_LeftListener DirectionDowm\n");
      }
    else
      {
	pd->moveButtonAction(evt,activeAxis,ExaktG::DirectionRight);
	printf("G_GUI::G_LeftListener DirectionRight\n");
      }
  }

  void G_GUI::G_SpeedListener(void* src,SDL_Event * evt)
  {
    G_GUI* pd = (G_GUI*)src;
    pd->incSpeedLevel();
  }

  void G_GUI::G_MoveBtnMouseOverListener(void * src,bool selected)
  {
    G_GUI* pd = (G_GUI*)src;
    ExaktG * pExaktG = pd->Parent->getExaktG();
    if(!selected){
      pExaktG->holdMoving(false);
    }
  }

  void G_GUI::incSpeedLevel(void)
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


  void G_GUI::moveButtonAction(SDL_Event * evt,int axis,int direction)
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

  void G_GUI::xPosLis(void * pLis,float pos)
  {
    static char pcTmp[16];
    G_GUI * pGTD;
    pGTD = (G_GUI *)pLis;
    if(ExaktG::AxisX==pGTD->getActiveAxis()){
      pGTD->theMenuG->pLPosition->setText(ExaktG::toString(pos,pcTmp,16));
      pGTD->theMenuG->pLPosition->show(pGTD->Parent->theGUI->getMainSurface());
    }
  }

  void G_GUI::yPosLis(void * pLis,float pos)
  {
    static char pcTmp[16];
    G_GUI * pGTD;
    pGTD = (G_GUI *)pLis;
    if(ExaktG::AxisY==pGTD->getActiveAxis()){
      pGTD->theMenuG->pLPosition->setText(ExaktG::toString(pos,pcTmp,16));
      pGTD->theMenuG->pLPosition->show(pGTD->Parent->theGUI->getMainSurface());
    }
  }

  void G_GUI::zPosLis(void * pLis,float pos)
  {
    static char pcTmp[16];
    G_GUI * pGTD;
    pGTD = (G_GUI *)pLis;
    pGTD->theMenuGZ->pLPosition->setText(ExaktG::toString(pos,pcTmp,16));
    pGTD->theMenuGZ->pLPosition->show(pGTD->Parent->theGUI->getMainSurface());
  }

  void G_GUI::aPosLis(void * pLis,float pos)
  {
    static char pcTmp[16];
    G_GUI * pGTD;
    pGTD = (G_GUI *)pLis;
    if(ExaktG::AxisA==pGTD->getActiveAxis()){
      pGTD->theMenuG->pLPosition->setText(ExaktG::toString(pos,pcTmp,16));
      pGTD->theMenuG->pLPosition->show(pGTD->Parent->theGUI->getMainSurface());
    }
  }

  void G_GUI::gFLis(void * pLis,int iA,int iB,int iC,int iD)
  {

  }


  void G_GUI::NextAxisButtonListener(void* src,SDL_Event * evt)
  {
    G_GUI * pG = (G_GUI*)src;
    pG->activateNextAxis();
  }

  void G_GUI::activateNextAxis(void)
  {
    int i = this->getActiveAxis();
    if(ExaktG::AxisA==i){
      this->activeAxis = ExaktG::AxisX;
      char * XText = (char*)"X-axis";
      theMenuG->pLName->setText((char*)XText);
    }else if(ExaktG::AxisX==i){
      this->activeAxis = ExaktG::AxisY;
      char * YText = (char*)"Y-axis";
      theMenuG->pLName->setText((char*)YText);
    }else{//if(ExaktG::AxisY==i)
      char * AText = (char*)"A-axis";
      theMenuG->pLName->setText((char*)AText);
      this->activeAxis = ExaktG::AxisA;
    }
    theMenuG->pLName->show(Parent->theGUI->getMainSurface());
  }

  int G_GUI::getActiveAxis(void)
  {
    return this->activeAxis;
  }

  G_GUI::G_GUI(char* text,		\
		       int sdlw,		\
		       int sdlh,		\
		       int camw,		\
		       int camh,		\
		       int yPos,\
			   ArbeitsDialog * parent)
  {
    this->Parent = parent;
    this->activeAxis = ExaktG::AxisX;

    this->tGCodeLis.pTheListener = this;
    this->tGCodeLis.fnkXPosLis =&this->xPosLis;
    this->tGCodeLis.fnkYPosLis =&this->yPosLis;
    this->tGCodeLis.fnkZPosLis =&this->zPosLis;
    this->tGCodeLis.fnkAPosLis =&this->aPosLis;
    this->tGCodeLis.fnkGFLis =&this->gFLis;

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
    short Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    //short Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    //horizontal acht Spalten
    int width = 1008;
    int x_space = 2;
    int Bw = (width -7*x_space)/8;
    int B1x = sdlw/2 - width/2;
    //int B2x = B1x + 1*Bw+1*x_space;
    //int B3x = B1x + 2*Bw+2*x_space;
    //int B4x = B1x + 3*Bw+3*x_space;
    //int B5x = B1x + 4*Bw+4*x_space;
    //int B6x = B1x + 5*Bw+5*x_space;
    int B7x = B1x + 6*Bw+6*x_space;
    int B8x = B1x + 7*Bw+7*x_space;

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

    pBNextAxis = new Button((char *)"NextAxis",
			    B7x+x_space,				\
			    (int)Zeile4_y,				\
			    Bw,						\
			    MZeile_h,					\
			    Parent->MenuSet);

    pBNextAxis->setLMButtonUpEvtHandler(NextAxisButtonListener);
    pBNextAxis->setPrivateMouseOver(0);
    pBNextAxis->pTSource = this;

    Parent->addEvtTarget(pBNextAxis);

  }

  void G_GUI::setActive(void)
  {
    this->Parent->getExaktG()->setGCodeResultListener(&this->tGCodeLis);
    this->Parent->getExaktG()->getG_Ctrl()->cmdGetStatus();
  }
}/* end Namespace */
