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
#include "ArbeitsDialog.h"

#include "MenuBar.h"

namespace EuMax01
{

  MenuBar::~MenuBar(){}
  MenuBar::MenuBar(int xPos,					\
		   int yPos,					\
		   int height,					\
		   char * Name,					\
		   char * LabelTexte[MenuBar::MenuPunkte],	\
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
	
	lbuf[i] = new Label(" ",				\
			    x1Button+wButton*i+x_space*i,	\
			    yPos,				\
			    wButton,				\
			    height,				\
			    Parent->MenuSet);			
	if(0!=LabelTexte[i])
	  {
	    lbuf[i]->setText(LabelTexte[i]);
	  }
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

}
