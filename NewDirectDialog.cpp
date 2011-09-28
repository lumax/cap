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

//#include "v4l_capture.h"
#include "Rezept.h"
//#include "Protocol.h"
//#include "pec_cmd.h"
//#include "MBProt.h"

#include "ArbeitsDialog.h"
#include "Dialoge.h"
#include "NewDirectDialog.h"

namespace EuMax01
{

  static void NewDirectKeyListener(void * src, SDL_Event * evt)
  {
    NewDirectDialog* ad = (NewDirectDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	printf("NewDirectKeyListener\n");
	/*	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->Parent->Parent->showArbeitsDialog();
	  }
	else if(key->keysym.sym == SDLK_RETURN)
	  {
	    ad->resetStepValue();
	    ad->incStep();
	  }
	else */if(key->keysym.sym == SDLK_LEFT)
	  {
	    ad->decEingabeSchritt();
	  }
	else if(key->keysym.sym == SDLK_RIGHT)
	  {

	    ad->incEingabeSchritt();
	    }
      }
  }

  void* NewDirectDialog::getKeyListener()
  {
    return (void*)NewDirectKeyListener;
  }

  NewDirectDialog::~NewDirectDialog(){};
  NewDirectDialog::NewDirectDialog(int sdlw,				\
				   int sdlh,				\
				   int camw,				\
				   int camh,				\
				   int yPos,NewDialog * parent):Screen(), \
								TF_Len(6)
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y;

    this->Parent = parent;
    this->ActualStep = 0;

    SchrittTexte[0] = (char *)"Schritt1";
    SchrittTexte[1] = (char *)"Schritt2";
    SchrittTexte[2] = (char *)"Schritt3";
    SchrittTexte[3] = (char *)"Schritt4";
    SchrittTexte[4] = (char *)"Schritt5";
    

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

    MLinks_x = sdlw/2 - 506;

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;


    snprintf(this->StepText,256,\
	     "Move camera 1 X-Axis in zero position :");
    Label_Step = new Label(this->StepText,			\
			   MLinks_x,Zeile1_y,506*2,MZeile_h,Parent->Parent->MenuSet);
    Label_Step->setFontColor(&Globals::GlobalSDL_Color3);//ein rot

    Label_ValueName = new Label(this->ValueName,			\
				MLinks_x,Zeile3_y,			\
				506-MSpace_h,MZeile_h);  
    Label_ValueName->setText(Parent->Parent->TextCam1Xaxis);

    snprintf(this->Value,64,"---");
    TF_Value = new TextField(Value,TF_Len,				\
			    MLinks_x+506+MSpace_h,			\
			    Zeile3_y,506-MSpace_h,			\
			    MZeile_h,Parent->Parent->WerteSet);
    TF_Value->setBorder(true);

    Label_MenuTitle = new Label("Calibration",MLinks_x,Zeile5_y,150,MZeile_h,Parent->Parent->MenuSet);

    snprintf(this->InfoText,256,				       \
	     "RETURN : set zero position | "			       \
	     "LEFT previous step | RIGHT next step");
    Label_Menu = new Label(this->InfoText,			 \
			    MLinks_x+158,Zeile5_y,			 \
			   1012-158,MZeile_h,Parent->Parent->MenuSet);

    addEvtTarget(Label_Step);
    addEvtTarget(Label_ValueName);
    addEvtTarget(TF_Value);
    addEvtTarget(Label_MenuTitle);
    addEvtTarget(Label_Menu);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(NewDirectKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
  }

  void NewDirectDialog::incEingabeSchritt()
  {
    ActualStep++;
    if(ActualStep>=NewDirectDialog::AnzahlEingabeSchritte)
      {
	ActualStep = NewDirectDialog::AnzahlEingabeSchritte-1;
      }
    this->showEingabeSchritt();
  }

  void NewDirectDialog::useNewDirectDialog(PositionSet * thePositionSet)
  {
    this->ActualStep = 0;
    this->showEingabeSchritt();
  }

  void NewDirectDialog::decEingabeSchritt()
  {
    if(ActualStep>0)
      {
	ActualStep--;
      }
    this->showEingabeSchritt();
  }

  void NewDirectDialog::showEingabeSchritt()
  {
    
    //Wert Anzeige resetten
    snprintf(this->Value,64,"---");
    this->TF_Value->setText(this->Value);
    Label::showLabel((void*)this->TF_Value,			\
		     this->Parent->Parent->theGUI->getMainSurface());

    this->Label_Step->setText(SchrittTexte[ActualStep]);

    snprintf(this->ValueName,16,"Z2");
    this->Label_ValueName->setText(this->ValueName);
    
    Label::showLabel((void*)this->Label_Step,			\
		     this->Parent->Parent->theGUI->getMainSurface());
    Label::showLabel((void*)this->Label_ValueName,		\
		     this->Parent->Parent->theGUI->getMainSurface());
  }
  
  /*  void NewDirectDialog::setQ1(unsigned short dat)
  {
    setXXData(dat,NewDirectDialog::iQ1,(char*)" mm");  
  }

  void NewDirectDialog::setQ2(unsigned short dat)
  {
    setXXData(dat,NewDirectDialog::iQ2,(char*)" mm");    
  }

  void NewDirectDialog::setZ1(unsigned short dat)
  {
    setXXData(dat,NewDirectDialog::iZ1,(char*)" ° ");    
  }
  */
  void NewDirectDialog::setXXData(unsigned short dat,int MyStep,char*suffix)
  {
    if(MyStep==this->ActualStep)
      {
	sprintf(this->Value,"%7.2f%s",(float)dat/100,suffix);
	this->TF_Value->setText(this->Value);
	Label::showLabel((void*)this->TF_Value,		\
			 this->Parent->Parent->theGUI->getMainSurface());
      }
  }


}
