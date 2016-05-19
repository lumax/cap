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

#include "G_NewDialog.h"

namespace EuMax01
{

  G_NewDialog::G_NewDialog(int sdlw,	\
			   int sdlh,			\
			   int camw,			\
			   int camh,					\
			   int yPos,					\
			   ArbeitsDialog * parent):			\
    NewDialog(sdlw,						\
	      sdlh,							\
	      camw,							\
	      camw,							\
	      yPos,parent)
  {
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
    //short Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
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
    int B3x = B1x + 2*Bw+2*x_space;
    //int B4x = B1x + 3*Bw+3*x_space;
    int B5x = B1x + 4*Bw+4*x_space;
    //int B6x = B1x + 5*Bw+5*x_space;
    int B7x = B1x + 6*Bw+6*x_space;
    //int B8x = B1x + 7*Bw+7*x_space;

    Label_Name->setWidth(Bw*2);
    LabelXaxisText->setWidth(Bw*2);
    LabelZaxisText->setWidth(Bw*2);
    LabelCrossText->setWidth(Bw*2);

    TextField_Name->setWidth(Bw*2);
    TextField_Name->setPosX(B3x+x_space);
    LabelWerte[NewDialog::iPosX1]->setWidth(Bw*2);
    LabelWerte[NewDialog::iPosX2]->setWidth(Bw*2);
    LabelWerte[NewDialog::iPosZ]->setWidth(Bw*2);
    LabelWerte[NewDialog::iPosX1]->setPosX(B3x+x_space);
    LabelWerte[NewDialog::iPosX2]->setPosX(B3x+x_space);
    LabelWerte[NewDialog::iPosZ]->setPosX(B3x+x_space);

    Label_Info->setWidth(Bw*2);
    Label_Info->setPosX(B5x+x_space);
    LabelRezept[NewDialog::iPosX1]->setWidth(Bw*2);
    LabelRezept[NewDialog::iPosX2]->setWidth(Bw*2);
    LabelRezept[NewDialog::iPosZ]->setWidth(Bw*2);
    LabelRezept[NewDialog::iPosX1]->setPosX(B5x+x_space);
    LabelRezept[NewDialog::iPosX2]->setPosX(B5x+x_space);
    LabelRezept[NewDialog::iPosZ]->setPosX(B5x+x_space);

    pG_GUI = new G_GUI(parent,this,B7x+x_space,yPos+MSpace_h,Bw,MZeile_h);
  }

  void G_NewDialog::setActive(void)
  {
    pG_GUI->setActive();
    //this->Parent->getExaktG()->setGCodeResultListener(&this->tGCodeLis);
    //this->pGCtrl->cmdGetStatus();
  }
}/* end Namespace */
