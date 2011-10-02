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

  void NewDirectDialog::NewDirectKeyListener(void * src, SDL_Event * evt)
  {
    NewDirectDialog* ad = (NewDirectDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    char zeichen;
    if( key->type == SDL_KEYUP )
      {
	zeichen = Tool::getIntegerNumeric_Char(key);
	if(ad->JungfreulicheEingabe&&zeichen!=0)
	  {
	    ad->TF_Value->setText((char *)"");
	    ad->TF_Value->addChar(zeichen);
	    ad->JungfreulicheEingabe = false;
	  }

	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    printf("NewDirectKeyListener ESCAPE\n");
	    ad->Parent->newDirectReturn(0);
	  }
	else if(key->keysym.sym == SDLK_RETURN)
	  {
	    ad->confirmValue(atof(ad->TF_Value->getText()));
	    ad->incEingabeSchritt();
	    //ad->showEingabeSchritt();
	  }
	else if(key->keysym.sym == SDLK_LEFT)
	  {
	    ad->decEingabeSchritt();
	  }
	else if(key->keysym.sym == SDLK_RIGHT)
	  {

	    ad->incEingabeSchritt();
	    }
      }
  }

  NewDirectDialog::~NewDirectDialog(){};
  NewDirectDialog::NewDirectDialog(int sdlw,				\
				   int sdlh,				\
				   int camw,				\
				   int camh,				\
				   int yPos,NewDialog * parent):Screen(), \
								TF_Len(24)
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
    this->thePosSet.cams[0].x_pos = 0;
    this->thePosSet.cams[0].z_pos = 0;
    this->thePosSet.cams[0].x_cross = 0;
    this->thePosSet.cams[1].x_pos = 0;
    this->thePosSet.cams[1].z_pos = 0;
    this->thePosSet.cams[1].x_cross = 0;
    JungfreulicheEingabe = true;

    SchrittTexte[0] = (char *)"Enter x-axis position for cam 1:";
    SchrittTexte[1] = (char *)"Enter x-axis position for cam 2:";
    SchrittTexte[2] = (char *)"Enter z-axis position:";
    SchrittTexte[3] = (char *)"Enter crossair position in percent:";
    SchrittTexte[4] = (char *)"Enter crossair position in percent:";
    
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

    Label_ValueName = new Label(this->ValueName,			\
				MLinks_x,Zeile3_y,			\
				506-MSpace_h,MZeile_h);  
    Label_ValueName->setText(Parent->Parent->TextCam1Xaxis);

    snprintf(this->Value,64," ");
    TF_Value = new TextField(Value,TF_Len,				\
			    MLinks_x+506+MSpace_h,			\
			    Zeile3_y,506-MSpace_h,			\
			    MZeile_h,Parent->Parent->WerteSet);
    TF_Value->activateKeyListener(TextField::IntegerNumericChar);
    TF_Value->setBorder(true);
    TF_Value->setActive(true);

    Label_MenuTitle = new Label("Direct Input",MLinks_x,Zeile5_y,150,MZeile_h,Parent->Parent->MenuSet);

    snprintf(this->InfoText,256,				       \
	     "ESC : cancel"
	     "RETURN : confirm value | "			       \
	     "LEFT previous step | RIGHT next step");
    Label_Menu = new Label(this->InfoText,			 \
			    MLinks_x+158,Zeile5_y,			 \
			   1012-158,MZeile_h,Parent->Parent->MenuSet);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(NewDirectKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!

    addEvtTarget(Label_Step);
    addEvtTarget(Label_ValueName);
    addEvtTarget(TF_Value);
    addEvtTarget(Label_MenuTitle);
    addEvtTarget(Label_Menu);
  }

  void NewDirectDialog::incEingabeSchritt()
  {
    this->ActualStep++;
    if(this->ActualStep>=NewDirectDialog::AnzahlEingabeSchritte)
      {
	this->ActualStep = NewDirectDialog::AnzahlEingabeSchritte-1;
      }
    this->showEingabeSchritt();
  }

  void NewDirectDialog::useNewDirectDialog(PositionSet * thePositionSet)
  {
    this->ActualStep = 0;
    printf("vor memcpy 0\n");
    if(thePositionSet)
      memcpy(&this->thePosSet,thePositionSet,sizeof(PositionSet));
    printf("nach memcpy 0\n");
    this->showEingabeSchritt();
  }

  void NewDirectDialog::decEingabeSchritt()
  {
    if(this->ActualStep>0)
      {
	this->ActualStep--;
      }
    this->showEingabeSchritt();
  }

  void NewDirectDialog::getSchritteValueNames(char * buf,int len)
  {
    if(0==this->ActualStep)
      {
	snprintf(buf,len,"Cam 1 X-axis");
      }
    else if(1==this->ActualStep)
      {
	snprintf(buf,len,"Cam 2 X-axis");
      }
    else if(2==this->ActualStep)
      {
	snprintf(buf,len,"Z-axis");
      }
    else if(3==this->ActualStep)
      {
	snprintf(buf,len,"Cam 1 cross");
      }
    else if(4==this->ActualStep)
      {
	snprintf(buf,len,"Cam 2 cross");
      }
    else
      {
	snprintf(buf,len,"N/A");
      }
  }

  void NewDirectDialog::getSchritteValues(char * buf,int len)
  {
    if(0==this->ActualStep)
      {
	snprintf(buf,len,"%7.2f mm",(float)thePosSet.cams[0].x_pos/100);
      }
    else if(1==this->ActualStep)
      {
	snprintf(buf,len,"%7.2f mm",(float)thePosSet.cams[1].x_pos/100);
      }
    else if(2==this->ActualStep)
      {
	snprintf(buf,len,"%7.2f °",(float)thePosSet.cams[0].z_pos/100);
      }
    else if(3==this->ActualStep)
      {
	snprintf(buf,len,"%i",thePosSet.cams[0].x_cross);
      }
    else if(4==this->ActualStep)
      {
	snprintf(buf,len,"%i",thePosSet.cams[1].x_cross);
      }
    else
      {
	snprintf(buf,len,"N/A");
      }
  }

  void NewDirectDialog::confirmValue(int val)
  {
    if(0==this->ActualStep)
      {
	thePosSet.cams[0].x_pos = val;
      }
    else if(1==this->ActualStep)
      {
	thePosSet.cams[1].x_pos = val;
      }
    else if(2==this->ActualStep)
      {
	thePosSet.cams[0].z_pos = val;
      }
    else if(3==this->ActualStep)
      {
	thePosSet.cams[0].x_cross = val;
      }
    else if(4==this->ActualStep)
      {
	thePosSet.cams[1].x_cross = val;
      }
    else
      {

      }
  }

  void NewDirectDialog::showEingabeSchritt()
  {
    JungfreulicheEingabe = true;
    getSchritteValues(this->Value,64);
    this->TF_Value->setText(this->Value);
    Label::showLabel((void*)this->TF_Value,			\
		     this->Parent->Parent->theGUI->getMainSurface());

    this->Label_Step->setText(SchrittTexte[this->ActualStep]);

    this->getSchritteValueNames(this->ValueName,16);
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
