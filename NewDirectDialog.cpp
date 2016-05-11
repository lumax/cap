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
//#include "Protocol.h"
//#include "pec_cmd.h"
//#include "MBProt.h"

#include "MenuBar.h"
#include "ArbeitsDialog.h"
#include "Dialoge.h"
#include "NewDirectDialog.h"

namespace EuMax01
{

  void NewDirectDialog::escape_listener(void * src, SDL_Event * evt)
  {
    NewDirectDialog* ad = (NewDirectDialog*)src;//KeyListener
    ad->Parent->newDirectReturn(0);
  }

  void NewDirectDialog::left_listener(void * src, SDL_Event * evt)
  {
    NewDirectDialog* ad = (NewDirectDialog*)src;//KeyListener
    /*    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    SDLMod mod = key->keysym.mod;
    if((mod & KMOD_SHIFT) || (mod & KMOD_ALT))
      {
	ad->crosshairsKeyListener(key);
      }
    else
    {*/
    ad->decEingabeSchritt();
	// }
  }

  void NewDirectDialog::right_listener(void * src, SDL_Event * evt)
  {
    NewDirectDialog* ad = (NewDirectDialog*)src;//KeyListener
    /*    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    SDLMod mod = key->keysym.mod;
    if((mod & KMOD_SHIFT) || (mod & KMOD_ALT))
      {
	ad->crosshairsKeyListener(key);
      }
    else
    {*/
    ad->incEingabeSchritt();
	// }
  }
  
  void NewDirectDialog::return_listener(void * src, SDL_Event * evt)
  {
    NewDirectDialog* ad = (NewDirectDialog*)src;//KeyListener
    if(ad->getEingabeSchritt()==NewDirectDialog::AnzahlEingabeSchritte-1)
      ad->Parent->newDirectReturn(ad->getPositionSet());
    else
      ad->incEingabeSchritt();
  }

  void NewDirectDialog::NewDirectKeyListener(void * src, SDL_Event * evt)
  {
    NewDirectDialog* ad = (NewDirectDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    //SDLMod mod = key->keysym.mod;
    char zeichen = 0;

    if( key->type == SDL_KEYUP )
      {
	zeichen = Tool::getIntegerNumeric_Char(key);
	if(key->keysym.sym == SDLK_BACKSPACE||zeichen!=0)
	  {
	    ad->confirmValue(atof(ad->TF_Value->getText()));
	    ad->getSchritteValues(ad->OldValue,64);
	    ad->Label_OldValue->setText(ad->OldValue);
	  }
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->escape_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->return_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_LEFT || key->keysym.sym == SDLK_F5)
	  {
	    ad->left_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RIGHT || key->keysym.sym == SDLK_F6)
	  {
	    ad->right_listener(src,evt);
	  }
      }
  }

  //static char * MenuAxisText = (char*)"ESC : cancel | RET : OK | LEFT prev | RIGHT next";
  //Dieser Text wird zur Zeit nicht erreicht, da AnzahlEingabeSchritte auf 3 begrenzt ist
  //static char * MenuCrossText = (char*)"ESC : cancel | RET : OK | LEFT prev | RIGHT next | Ctrl/Alt LEFT/RIGHT cross";

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
    short Zeile1_y,Zeile3_y,Zeile5_y,x1,x2,x3,x_space,button_breit,button_schmal;

    this->Parent = parent;
    this->ActualStep = 0;
    this->thePosSet.cams[0].x_pos = 0;
    this->thePosSet.cams[0].z_pos = 0;
    this->thePosSet.cams[0].x_cross = 0;
    this->thePosSet.cams[1].x_pos = 0;
    this->thePosSet.cams[1].z_pos = 0;
    this->thePosSet.cams[1].x_cross = 0;

    this->theStep = 1;

    SchrittTexte[0] = (char *)"Enter x-axis position cam 1 for step";
    SchrittTexte[1] = (char *)"Enter x-axis position cam 2 for step";
    SchrittTexte[2] = (char *)"Enter z-axis position for step";
    SchrittTexte[3] = (char *)"Enter crossair position in percent for step";
    SchrittTexte[4] = (char *)"Enter crossair position in percent for step";
    
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

    MLinks_x = sdlw/2 - 504;

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    //Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    //Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    button_breit = 400;
    button_schmal = 200;
    x_space = 6;
    x1 = MLinks_x;
    x2 = x1 + button_breit + x_space;
    x3 = x2 + button_schmal + x_space;

    snprintf(this->StepText,256,\
	     "Move camera 1 X-Axis in zero position :");
    Label_Step = new Label(this->StepText,			\
			   MLinks_x,Zeile1_y,504*2,MZeile_h,Parent->Parent->MenuSet);

    Label_ValueName = new Label(this->ValueName,			\
				x1,Zeile3_y,				\
				button_breit,MZeile_h);  
    Label_ValueName->setText(Parent->Parent->TextCam1Xaxis);

    TF_Value = new TextField("",TF_Len,					\
			     x2,					\
			     Zeile3_y,button_schmal,			\
			     MZeile_h,Parent->Parent->WerteSet);
    TF_Value->activateKeyListener(TextField::IntegerNumericChar);
    TF_Value->setBorder(false);
    TF_Value->hide(true);
    TF_Value->setActive(true);

    Label_OldValue = new Label(" -- ",			\
			       x3,			\
			       Zeile3_y,		\
			       button_breit,		\
			       MZeile_h,		\
			       Parent->Parent->WerteSet);

    //Label_MenuTitle = new Label("Direct Input",MLinks_x,Zeile5_y,150,MZeile_h,Parent->Parent->MenuSet);

    /*Label_Menu = new Label(" ",					\
			    MLinks_x+158,Zeile5_y,			 \
			   1008-158,MZeile_h,Parent->Parent->MenuSet);
    Label_Menu->setText(MenuAxisText);
    */

    theMenuBarSettings.Text[0]=0;
    theMenuBarSettings.Text[1]=0;
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=0;
    theMenuBarSettings.Text[4]=(char *)"F5 prev";
    theMenuBarSettings.Text[5]=(char *)"F6 next";
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=0;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=0;
    theMenuBarSettings.evtFnks[4]=left_listener;
    theMenuBarSettings.evtFnks[5]=right_listener;
    theMenuBarSettings.evtFnks[6]=escape_listener;
    theMenuBarSettings.evtFnks[7]=return_listener;

    theMenu = new MenuBar((int)MLinks_x,(int)Zeile5_y,(int)MZeile_h,(char*)"Direct input", \
			  &this->theMenuBarSettings,Parent->Parent);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->EvtTargetID=(char*)"NewDirectDialog";
    this->setKeyboardUpEvtHandler(NewDirectKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!

    addEvtTarget(Label_Step);
    addEvtTarget(Label_ValueName);
    addEvtTarget(TF_Value);
    addEvtTarget(Label_OldValue);
    //addEvtTarget(Label_MenuTitle);
    //addEvtTarget(Label_Menu);
    theMenu->addToEvtTarget(this);
  }

  PositionSet * NewDirectDialog::getPositionSet()
  {
    return &thePosSet;
  }

  void NewDirectDialog::incEingabeSchritt()
  {
    if(this->ActualStep<NewDirectDialog::AnzahlEingabeSchritte-1)
      {
	this->ActualStep++;
	TF_Value->setText((char *)"");//hidden TextField reset	
      }
    this->showEingabeSchritt();
  }

  int NewDirectDialog::getEingabeSchritt()
  {
    return this->ActualStep;
  }

  void NewDirectDialog::useNewDirectDialog(PositionSet * thePositionSet,int step)
  {
    this->theStep = step;
    this->ActualStep = 0;
    TF_Value->setText((char *)"");//hidden TextField reset
    if(thePositionSet)
      memcpy(&this->thePosSet,thePositionSet,sizeof(PositionSet));
    this->showEingabeSchritt();
  }

  void NewDirectDialog::decEingabeSchritt()
  {
    if(this->ActualStep>0)
      {
	this->ActualStep--;
	TF_Value->setText((char *)"");//hidden TextField reset
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
	//printf("getSchritteValues 2==this->ActualStep\n");
	snprintf(buf,len,"%7.2f mm",(float)thePosSet.cams[0].z_pos/100);
	//*this->Parent->Parent->getFaktorZAchse());
      }
    else if(3==this->ActualStep)
      {
	//printf("Cam1 thePosSet.cams[0].x_cross %i\n",thePosSet.cams[0].x_cross);
	//printf("Cam2 thePosSet.cams[1].x_cross %i\n",thePosSet.cams[1].x_cross);
	//snprintf(buf,len,"%i",Parent->Parent->getCrossInPercent(0,thePosSet.cams[0].x_cross));
      }
    else if(4==this->ActualStep)
      {
	//snprintf(buf,len,"%i",thePosSet.cams[1].x_cross);
	//snprintf(buf,len,"%i",Parent->Parent->getCrossInPercent(1,thePosSet.cams[1].x_cross));
      }
    else
      {
	snprintf(buf,len,"N/A");
      }
  }

  void NewDirectDialog::confirmValue(int val)
  {
    //printf("confirmValue Wert:%i\n",val);
    if(0==this->ActualStep)
      {
	if(val<0)
	  val = 0;
	//if(val>0xffff)
	//  val=0xffff;
	thePosSet.cams[0].x_pos = val;
      }
    else if(1==this->ActualStep)
      {
	if(val<0)
	  val = 0;
	//if(val>0xffff)
	//  val=0xffff;
	thePosSet.cams[1].x_pos = val;
      }
    else if(2==this->ActualStep)
      {
	if(val<0)
	  val = 0;
	//if(val>0xffff)
	//  val=0xffff;
	thePosSet.cams[0].z_pos = val;
      }
    else if(3==this->ActualStep)
      {
	/*	if(val<=0)
	  val=0;
	if(val>=100)
	  val=100;
	cap_cam_setCrossX(0,val);
	thePosSet.cams[0].x_cross = cap_cam_getCrossX(0);*/
      }
    else if(4==this->ActualStep)
      {
	/*	cap_cam_setCrossX(1,val);
		thePosSet.cams[1].x_cross = cap_cam_getCrossX(1);*/
      }
    else
      {

      }
  }

  void NewDirectDialog::showEingabeSchritt()
  {
    /*    if(this->ActualStep==3||this->ActualStep==4)//cross_x hat kein TextFeld
      {
	Label_Menu->setText(MenuCrossText);
	this->Label_OldValue->hide(true);
	this->Parent->Parent->blankButton(Label_OldValue);
      }
    else
    {*/
    //Label_Menu->setText(MenuAxisText);
	this->Label_OldValue->hide(false);
	getSchritteValues(this->OldValue,64);
	this->Label_OldValue->setText(this->OldValue);
	this->Label_OldValue->show(this->Parent->Parent->theGUI->getMainSurface());
	//}

	snprintf(this->StepText,256,"%s %i :",SchrittTexte[this->ActualStep],this->theStep);
	this->Label_Step->setText(this->StepText);

    this->getSchritteValueNames(this->ValueName,16);
    this->Label_ValueName->setText(this->ValueName);
    
    this->Label_Step->show(this->Parent->Parent->theGUI->getMainSurface());
    this->Label_ValueName->show(this->Parent->Parent->theGUI->getMainSurface());
  }

  void NewDirectDialog::setXXData(unsigned short dat,int MyStep,char*suffix)
  {
    if(MyStep==this->ActualStep)
      {
	sprintf(this->Value,"%7.2f%s",(float)dat/100,suffix);
	this->TF_Value->setText(this->Value);
	this->TF_Value->show(this->Parent->Parent->theGUI->getMainSurface());
      }
  }

  /*  void NewDirectDialog::crosshairsKeyListener(SDL_KeyboardEvent * key)
  {
    SDLMod mod = key->keysym.mod;
    int cam = 0;

    if(this->ActualStep==3)
      cam = 0;
    else if(this->ActualStep==4)
      cam = 1;
    else
      return;//kein cross-Eingabe Dialog

    if(key->keysym.sym == SDLK_LEFT)
      {
	if((mod & KMOD_SHIFT)&&(mod & KMOD_ALT))
	  {
	    cap_cam_addCrossX(cam,-100);
	  }
	else if((mod & KMOD_SHIFT))
	  {
	    cap_cam_addCrossX(cam,-10);
	  }
	else if((mod & KMOD_ALT))
	  {
	    cap_cam_addCrossX(cam,-2);
	  }
      }
    else if(key->keysym.sym == SDLK_RIGHT)
      {
	if((mod & KMOD_SHIFT)&&(mod & KMOD_ALT))
	  {
	    cap_cam_addCrossX(cam,100);
	  }
	else if((mod & KMOD_SHIFT))
	  {
	    cap_cam_addCrossX(cam,10);
	  }
	else if((mod & KMOD_ALT))
	  {
	    cap_cam_addCrossX(cam,2);
	  }
      }
      }*/

}
