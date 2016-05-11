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

  MenuGLift::~MenuGLift(){}
  MenuGLift::MenuGLift(int xPos,				\
		       int yPos,				\
		       int y_height,				\
		       int y_space,				\
		       int width,				\
		       struct t_MenuGLiftSettings * settings,	\
		       ArbeitsDialog * Parent)
  {
    //short xPos[MenuBar::LabelBufLen];
    short x_space;
    int wHalbe,wButton,wButtonHalbe;

    x_space = 4;
    wHalbe = width/2;
    wButton = width;
    wButtonHalbe = wHalbe-x_space/2;

    pLName = new Label("Lift",xPos,yPos,wButton,y_height,Parent->DialogSet);

    pBUp = new Button("PgUp",						\
		      xPos,						\
		      yPos+y_height*1+y_space*1,					\
		      wButton,						\
		      y_height,						\
		      Parent->MenuSet);

    pBDown = new Button("PgDown",					\
			xPos,						\
			yPos+y_height*2+y_space*2,			\
			wButton,					\
			y_height,					\
			Parent->MenuSet);

    pBSetSpeed = new Button("Speed",					\
			    xPos,					\
			    yPos+y_height*3+y_space*3,			\
			    wButtonHalbe,				\
			    y_height,					\
			    Parent->MenuSet);

    pLSpeed = new Label("50km/h",					\
			xPos+wButtonHalbe+x_space,			\
			yPos+y_height*3+y_space*3,			\
			wButtonHalbe,					\
			y_height,					\
			Parent->WerteSet);


    pBUp->setLMButtonUpEvtHandler(settings->evtFnkUp);
    pBDown->setLMButtonUpEvtHandler(settings->evtFnkDown);
    pBSetSpeed->setLMButtonUpEvtHandler(settings->evtFnkSetSpeed);

    pBUp->pTSource = settings->evtSource;
    pBDown->pTSource = settings->evtSource;
    pBSetSpeed->pTSource = settings->evtSource;
  }
  
  void MenuGLift::addToEvtTarget(EvtTarget* theTarget)
  {
    theTarget->addEvtTarget(pLName);
    theTarget->addEvtTarget(pLSpeed);
    theTarget->addEvtTarget(pBUp);
    theTarget->addEvtTarget(pBDown);
    theTarget->addEvtTarget(pBSetSpeed);
  }

  void MenuGLift::setMenuName(char * theName)
  {
    if(0!=theName)
      {
	pLName->setText(theName);
      }
  }

  void MenuGLift::updateSettings(struct t_MenuBarSettings * settings)
  {/*
    for(int i = 0;i<MenuGLift::MenuPunkte;i++)
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
}//end namespace
