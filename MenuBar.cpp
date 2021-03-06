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
#include "v4l_capture.h"
#include "Rezept.h"
#include "Protocol.h"
#include "pec_cmd.h"
#include "MBProt.h"


//#include "Dialoge.h"
//#include "ErrorDialog.h"
#include "MenuBar.h"
#include "ArbeitsDialog.h"

namespace EuMax01
{

  MenuBar::~MenuBar(){}
  MenuBar::MenuBar(int xPos,					\
		   int yPos,					\
		   int height,					\
		   char * Name,					\
		   struct t_MenuBarSettings * settings,		\
		   ArbeitsDialog * Parent)
  {
    //short xPos[MenuBar::LabelBufLen];
    short x_space,x1Button;
    int wName,wButton;

    x_space = 4;
    wName = 149;
    wButton = 104;
    x1Button = xPos + wName + x_space;

    LName = new Label(Name,xPos,yPos,wName,height,Parent->MenuSet);

    for(int i = 0;i<MenuBar::MenuPunkte;i++)
      {
	
	lbuf[i] = new Button(" ",				\
			    x1Button+wButton*i+x_space*i,	\
			    yPos,				\
			    wButton,				\
			    height,				\
			    Parent->MenuSet);			
	if(0!=settings->Text[i])
	  {
	    lbuf[i]->setText(settings->Text[i]);
	  }
	if(0!=settings->evtFnks[i])
	  {
	    lbuf[i]->setLMButtonUpEvtHandler(settings->evtFnks[i]);//,settings->evtSource);
	  }
	lbuf[i]->pTSource = settings->evtSource;
      }

  }
  
  void MenuBar::addToEvtTarget(EvtTarget* theTarget)
  {
    theTarget->addEvtTarget(LName);
    for(int i = 0;i<MenuBar::MenuPunkte;i++)
      {
	theTarget->addEvtTarget(this->lbuf[i]);
      }
  }

  void MenuBar::setMenuName(char * theName)
  {
    if(0!=theName)
      {
	LName->setText(theName);
      }
  }

  void MenuBar::updateSettings(struct t_MenuBarSettings * settings)
  {
    for(int i = 0;i<MenuBar::MenuPunkte;i++)
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
      }
  }

  LoadMenuBar::~LoadMenuBar(){}
  LoadMenuBar::LoadMenuBar(int xPos,				\
			   int yPos,				\
			   int height,				\
			   char * Name,				\
			   struct t_MenuBarSettings * settings,	\
			   ArbeitsDialog * Parent,\
			   int MaxFilterTextLen):MenuBar(xPos,yPos,height, \
							   Name,settings,Parent)
  {

    short x_space,x1Button;
    int wName,wButton;

    x_space = 4;
    wName = 149;
    wButton = 104;
    x1Button = xPos + wName + x_space;

    TextField_Filter = new TextField(0,MaxFilterTextLen,	\
				     x1Button+wButton*2+x_space*2,	\
				     yPos,\
				     wButton+x_space+wButton,				\
				     height,				\
				     Parent->WerteSet);
    TextField_Filter->setFont(Globals::getFontButtonBig());
    TextField_Filter->setActive(true);
    TextField_Filter->hide(false);
  }

  void LoadMenuBar::addToEvtTarget(EvtTarget* theTarget)
  {
    theTarget->addEvtTarget(LName);
    theTarget->addEvtTarget(this->lbuf[0]);
    theTarget->addEvtTarget(this->lbuf[1]);
    //Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    //Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    theTarget->addEvtTarget(TextField_Filter);
    for(int i = 4;i<MenuBar::MenuPunkte;i++)
      {
	theTarget->addEvtTarget(this->lbuf[i]);
      }
  }

}
