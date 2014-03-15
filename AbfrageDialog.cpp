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
#include <SDL/SDL_image.h>
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

#include "ErrorDialog.h"
#include "AbfrageDialog.h"

namespace EuMax01
{

  static void TasteEnter(void * src, SDL_Event * evt)
  {
    AbfrageDialog* ad = (AbfrageDialog*)src;
    if(remove(ad->pPathToFile))
      {
	printf( "Error deleting file: %s\n", strerror( errno ) );
	ad->Parent->showErrorDialog((char*)"Error Deleting Recipe File");
      }
    else
      {
	ad->Parent->showLoadDialog(0,false);
      }
  }
  
  static void TasteEscape(void * src, SDL_Event * evt)
  {
    AbfrageDialog* ad = (AbfrageDialog*)src;
    ad->Parent->showLoadDialog(0,false);
  }

  static void AbfrageDialogKeyListener(void * src, SDL_Event * evt)
  {
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    TasteEscape(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    TasteEnter(src,evt);
	  }
	//else if(key->keysym.sym == SDLK_F1)
	//{
	//  TasteF1(src,evt);
	//}
	//else if(key->keysym.sym == SDLK_F2)
	//{
	//  TasteF2(src,evt);
	//}
      }
  }

  AbfrageDialog::~AbfrageDialog(){}
  AbfrageDialog::AbfrageDialog(int sdlw,		\
			     int sdlh,		\
			     int camw,					\
			     int camh,					\
			       int yPos,ArbeitsDialog * parent):Screen(),\
								TF_Len(32)
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile3_y,Zeile5_y,x1,x2,x_space,button_breit,button_schmal;

    this->Parent = parent;

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
    //x3 = x2 + button_schmal + x_space;

    this->Parent = parent;
    this->pPathToFile = 0;

    Label_Info = new Label("Are sou sure you want to delete file:",\
			   MLinks_x,Zeile1_y,506*2,MZeile_h,Parent->DialogSet);
    /*			   Spalte1_x,		\
			   Zeile1_y,\
			   506*2,\
			   MZeile_h,\
			   Parent->DialogSet);*/

    TF_Value = new TextField("",TF_Len,					\
			     x2,					\
			     Zeile3_y,button_schmal,			\
			     MZeile_h,Parent->WerteSet);
    TF_Value->activateKeyListener(TextField::IntegerNumericChar);
    TF_Value->setBorder(true);
    TF_Value->hide(false);
    TF_Value->setActive(true);

    snprintf(Value,TF_Len,"MYAmazingFile");
    TF_Value->setText(Value);

    theMenuBarSettings.Text[0]=0;//(char *)"F1service";
    theMenuBarSettings.Text[1]=0;//(char *)"F2 info";
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=0;
    theMenuBarSettings.Text[4]=0;
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;//TasteF1;
    theMenuBarSettings.evtFnks[1]=0;//TasteF2;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=0;
    theMenuBarSettings.evtFnks[4]=0;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=TasteEscape;
    theMenuBarSettings.evtFnks[7]=TasteEnter;

    theMenu = new MenuBar((int)MLinks_x,(int)Zeile5_y,(int)MZeile_h,(char*)"Delete file", \
			  &this->theMenuBarSettings,Parent);

    this->pTSource = this;//EvtTarget Quelle setzen
    this->EvtTargetID=(char*)"AbfrageDialog";
    this->setKeyboardUpEvtHandler(AbfrageDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    theMenu->addToEvtTarget(this);
    this->addEvtTarget(Label_Info);
    this->addEvtTarget(TF_Value);
  }

  void AbfrageDialog::setFileToDelete(char * path,char * filename)
  {
    this->pPathToFile = path;

    snprintf(Value,TF_Len,"%s",filename);
    TF_Value->setText(Value);
  }

}
