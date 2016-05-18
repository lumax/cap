/*
Bastian Ruppert
*/
//read directories
#include <stdio.h>

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

#include <termios.h>
#include <unistd.h>
//#include "v4l_capture.h"
#include "Rezept.h"
#include "Protocol.h"
#include "pec_cmd.h"
#include "MBProt.h"

#include "MenuBar.h"
#include "Dialoge.h"
#include "ErrorDialog.h"
#include "GMenu.h"
#include "ArbeitsDialog.h"

namespace EuMax01
{

  void MenuGBase::dummyMouseOver(void *,bool sel)
  {
    //printf("MenuGBase::dummySpeedMouseOver(void*,SDL_Event * evt)\n");
  }

  MenuGBase::~MenuGBase(){}
  MenuGBase::MenuGBase(char * name,				\
		       int xPos,				\
		       int yPos,				\
		       int y_height,				\
		       int y_space,				\
		       int width,				\
		       struct t_MenuGSettings * settings,	\
		       ArbeitsDialog * Parent)
  {
    this->theArbeitsDialog = Parent;
  }

  ArbeitsDialog * MenuGBase::getArbeitsDialog(){
    return this->theArbeitsDialog;
  }

  void MenuGBase::setMenuName(char * theName)
  {
    if(0!=theName)
      {
	pLName->setText(theName);
      }
  }

  void MenuGBase::updateSettings(struct t_MenuBarSettings * settings)
  {/*
    for(int i = 0;i<MenuGHorizontal::MenuPunkte;i++)
      {
	if(0!=settings->Text[i])
	  {
	    lbuf[i]->setText(settings->Text[i]);
	  }
	else
	  {
	    lbuf[i]->setText(" ");
	  }
	lbuf[i]->setLMButtonUpEvtHandler(settings->evtFnks[i]);//,settings->evtSource);
	}*/
  }


  MenuGHorizontal::~MenuGHorizontal(){}
  MenuGHorizontal::MenuGHorizontal(char * name,			\
				   int xPos,			\
				   int yPos,			\
				   int y_height,		\
				   int y_space,			\
				   int width,			\
				   struct t_MenuGSettings * settings,	\
				   ArbeitsDialog * Parent):		\
    MenuGBase(name,							\
	      xPos,							\
	      yPos,							\
	      y_height,							\
	      y_space,							\
	      width,							\
	      settings,							\
	      Parent)
{
    //short xPos[MenuBar::LabelBufLen];
    short x_space;
    int wHalbe,wButton,wButtonHalbe;

    x_space = 4;
    wHalbe = width/2;
    wButton = width;
    wButtonHalbe = wHalbe-x_space/2;

    pLName = new Label(name,\
		       xPos,yPos,wButton,y_height,Parent->DialogSet);

    pB1 = new Button(settings->btn1Text,				\
		      xPos,						\
		      yPos+y_height*1+y_space*1,			\
		      wButtonHalbe,					\
		      y_height,						\
		      Parent->MenuSet);

    pB2 = new Button(settings->btn2Text,				\
			xPos,						\
			yPos+y_height*2+y_space*2,			\
			wButtonHalbe,					\
			y_height,					\
			Parent->MenuSet);

    pBSetSpeed = new Button("Speed",					\
			    xPos,					\
			    yPos+y_height*3+y_space*3,			\
			    wButtonHalbe,				\
			    y_height,					\
			    Parent->MenuSet);

    pLSpeed = new Label(settings->SpeedLabelText,			\
			xPos+wButtonHalbe+x_space,			\
			yPos+y_height*3+y_space*3,			\
			wButtonHalbe,					\
			y_height,					\
			Parent->WerteSet);

    pLPosition = new Label("200mm",					\
			xPos+wButtonHalbe+x_space,			\
			yPos+y_height*1+y_space*1,			\
			wButtonHalbe,					\
			y_height,					\
			Parent->WerteSet);


    pB1->setLMButtonUpEvtHandler(settings->evtFnkBtn1Up);
    pB1->setLMButtonDownEvtHandler(settings->evtFnkBtn1Down);
    pB1->setPrivateMouseOver(settings->evtFnkBtn1MouseOver);
    pB2->setLMButtonUpEvtHandler(settings->evtFnkBtn2Up);
    pB2->setLMButtonDownEvtHandler(settings->evtFnkBtn2Down);
    pB2->setPrivateMouseOver(settings->evtFnkBtn2MouseOver);
    pBSetSpeed->setLMButtonUpEvtHandler(settings->evtFnkSetSpeed);
    pBSetSpeed->setPrivateMouseOver(dummyMouseOver);

    pB1->pTSource = settings->evtSource;
    pB2->pTSource = settings->evtSource;
    pBSetSpeed->pTSource = settings->evtSource;
}

  
  void MenuGHorizontal::addToEvtTarget(EvtTarget* theTarget)
  {
    theTarget->addEvtTarget(pLName);
    theTarget->addEvtTarget(pLSpeed);
    theTarget->addEvtTarget(pB1);
    theTarget->addEvtTarget(pB2);
    theTarget->addEvtTarget(pBSetSpeed);
    theTarget->addEvtTarget(pLPosition);
  }

  MenuGVertical::~MenuGVertical(){}
  MenuGVertical::MenuGVertical(char * name,			\
		       int xPos,				\
		       int yPos,				\
		       int y_height,				\
		       int y_space,				\
		       int width,				\
		       struct t_MenuGSettings * settings,	\
			       ArbeitsDialog * Parent):		\
    MenuGBase(name,							\
	      xPos,							\
	      yPos,							\
	      y_height,							\
	      y_space,							\
	      width,							\
	      settings,							\
	      Parent)
  {
    int x_space = 4;
    int wHalbe = width/2;
    int wViertel = width/4;
    int wButton = width;
    int wButtonHalbe = wHalbe-x_space/2;
    int wButtonViertel = wViertel-x_space/2;

    pLName = new Label(name,\
		       xPos,yPos,wButton,y_height,Parent->DialogSet);

    pB1 = new Button(settings->btn1Text,				\
		      xPos,						\
		      yPos+y_height*1+y_space*1,			\
		      wButtonViertel,					\
		      y_height,						\
		      Parent->MenuSet);

    pB2 = new Button(settings->btn2Text,				\
			xPos + wButtonViertel+x_space/2,		\
			yPos+y_height*1+y_space*1,			\
			wButtonViertel,					\
			y_height,					\
			Parent->MenuSet);

    pLPosition = new Label("200mm",					\
			xPos+wButtonHalbe+x_space,			\
			yPos+y_height*1+y_space*1,			\
			wButtonHalbe,					\
			y_height,					\
			Parent->WerteSet);

    pBSetSpeed = new Button("Speed",					\
			    xPos,					\
			    yPos+y_height*2+y_space*2,			\
			    wButtonHalbe,				\
			    y_height,					\
			    Parent->MenuSet);

    pLSpeed = new Label(settings->SpeedLabelText,			\
			xPos+wButtonHalbe+x_space,			\
			yPos+y_height*2+y_space*2,			\
			wButtonHalbe,					\
			y_height,					\
			Parent->WerteSet);


    pB1->setLMButtonUpEvtHandler(settings->evtFnkBtn1Up);
    pB1->setLMButtonDownEvtHandler(settings->evtFnkBtn1Down);
    pB1->setPrivateMouseOver(settings->evtFnkBtn1MouseOver);
    pB2->setLMButtonUpEvtHandler(settings->evtFnkBtn2Up);
    pB2->setLMButtonDownEvtHandler(settings->evtFnkBtn2Down);
    pB2->setPrivateMouseOver(settings->evtFnkBtn2MouseOver);
    pBSetSpeed->setLMButtonUpEvtHandler(settings->evtFnkSetSpeed);
    pBSetSpeed->setPrivateMouseOver(dummyMouseOver);

    pB1->pTSource = settings->evtSource;
    pB2->pTSource = settings->evtSource;
    pBSetSpeed->pTSource = settings->evtSource;
  }

  void MenuGVertical::addToEvtTarget(EvtTarget* theTarget)
  {
    theTarget->addEvtTarget(pLName);
    theTarget->addEvtTarget(pLSpeed);
    theTarget->addEvtTarget(pB1);
    theTarget->addEvtTarget(pB2);
    theTarget->addEvtTarget(pBSetSpeed);
    theTarget->addEvtTarget(pLPosition);
  }

}//end namespace
