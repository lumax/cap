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

#include "G_TestDialog.h"

namespace EuMax01
{
  void G_TestDialog::GX_LeftListener(void* src,SDL_Event * evt)
  {
    
  }

  void G_TestDialog::GX_RightListener(void* src,SDL_Event * evt)
  {
    
  }

  void G_TestDialog::GX_Btn1MouseOverListener(void*,bool selected)
  {
    printf("GX_Btn1MouseOverListener :%i\n",selected);
  }
  void G_TestDialog::GX_Btn2MouseOverListener(void*,bool selected)
  {
    printf("GX_Btn2MouseOverListener :%i\n",selected);
  }

  void G_TestDialog::GX_SpeedListener(void* src,SDL_Event * evt)
  {
    G_TestDialog* td = (G_TestDialog*)src;
    td->incSpeedLevel(ExaktG::AxisX);
  }

  void G_TestDialog::GY_LeftListener(void* src,SDL_Event * evt)
  {
    G_TestDialog* td = (G_TestDialog*)src;
    if(evt->type==SDL_MOUSEBUTTONDOWN)
      {
	//printf("SDL_MOUSEBUTTONDOWN\n");
      }
    if(evt->type==SDL_MOUSEBUTTONUP)
      {
	td->pExaktG->move(ExaktG::AxisY,ExaktG::DirectionLeft);
      }
  }


  void G_TestDialog::GY_RightListener(void* src,SDL_Event * evt)
  {
    G_TestDialog* td = (G_TestDialog*)src;
    if(evt->type==SDL_MOUSEBUTTONDOWN)
      {
	//printf("SDL_MOUSEBUTTONDOWN\n");
      }
    if(evt->type==SDL_MOUSEBUTTONUP)
      {
	td->pExaktG->move(ExaktG::AxisY,ExaktG::DirectionRight);
      }
  }

  void G_TestDialog::GY_Btn1MouseOverListener(void*,bool selected)
  {
    printf("GY_Btn1MouseOverListener :%i\n",selected);
  }
  void G_TestDialog::GY_Btn2MouseOverListener(void*,bool selected)
  {
    printf("GZ_Btn2MouseOverListener :%i\n",selected);
  }

  void G_TestDialog::GY_SpeedListener(void* src,SDL_Event * evt)
  {
    G_TestDialog* td = (G_TestDialog*)src;//KeyListener
    td->incSpeedLevel(ExaktG::AxisY);
  }

  void G_TestDialog::GA_UpListener(void* src,SDL_Event * evt)
  {
    printf("GA_UpListener\n");
  }

  void G_TestDialog::GA_DownListener(void* src,SDL_Event * evt)
  {
    
  }

  void G_TestDialog::GA_Btn1MouseOverListener(void*,bool selected)
  {
    printf("GA_Btn1MouseOverListener :%i\n",selected);
  }
  void G_TestDialog::GA_Btn2MouseOverListener(void*,bool selected)
  {
    printf("GA_Btn2MouseOverListener :%i\n",selected);
  }

  void G_TestDialog::GA_SpeedListener(void* src,SDL_Event * evt)
  {
    G_TestDialog* td = (G_TestDialog*)src;//KeyListener
    td->incSpeedLevel(ExaktG::AxisA);
  }

  void G_TestDialog::GLiftUpListener(void* src,SDL_Event * evt)
  {
    printf("G_TestDialog MenuGLiftUpListener\n");
  }

  void G_TestDialog::GZ_Btn1MouseOverListener(void*,bool selected)
  {
    printf("GZ_Btn1MouseOverListener :%i\n",selected);
  }
  void G_TestDialog::GZ_Btn2MouseOverListener(void*,bool selected)
  {
    printf("GZ_Btn2MouseOverListener :%i\n",selected);
  }

  void G_TestDialog::GLiftDownListener(void* src,SDL_Event * evt)
  {
    printf("G_TestDialog MenuGLiftDownListener\n");
  }

  void G_TestDialog::GLiftSetSpeedListener(void* src,SDL_Event * evt)
  {
    G_TestDialog* td = (G_TestDialog*)src;//KeyListener
    td->incSpeedLevel(ExaktG::AxisZ);
  }

  void G_TestDialog::escape_listener(void * src, SDL_Event * evt)
  {
    G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
    ad->Parent->showArbeitsDialog();
  }

  void G_TestDialog::left_listener(void * src, SDL_Event * evt)
  {
    //G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
    printf("G_TestDialog leftListener\n");
  }

  void G_TestDialog::right_listener(void * src, SDL_Event * evt)
  {
    //G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
     printf("G_TestDialog rightListener\n");
  }

  void G_TestDialog::return_listener(void * src, SDL_Event * evt)
  {
    G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
    if(ad->pTFEingabe->getTextLen()>0)
      {
	ad->pGCtrl->cmdG(ad->pTFEingabe->getText());
	ad->pTFEingabe->setText((char*)"");
	ad->pTFEingabe->show(ad->Parent->theGUI->getMainSurface());
      }
  }

  void G_TestDialog::getstatus_listener(void * src,SDL_Event * evt)
  {
    G_TestDialog* ad = (G_TestDialog*)src;
    ad->pGCtrl->cmdGetStatus();
  }

  void G_TestDialog::xPosLis(void * pLis,float pos)
  {
    static char pcTmp[16];
    G_TestDialog * pGTD;
    pGTD = (G_TestDialog *)pLis;
    pGTD->theMenuGX->pLPosition->setText(ExaktG::toString(pos,pcTmp,16));
    pGTD->theMenuGX->pLPosition->show(pGTD->Parent->theGUI->getMainSurface());
  }

  void G_TestDialog::yPosLis(void * pLis,float pos)
  {
    static char pcTmp[16];
    G_TestDialog * pGTD;
    pGTD = (G_TestDialog *)pLis;
    pGTD->theMenuGY->pLPosition->setText(ExaktG::toString(pos,pcTmp,16));
    pGTD->theMenuGY->pLPosition->show(pGTD->Parent->theGUI->getMainSurface());
  }

  void G_TestDialog::zPosLis(void * pLis,float pos)
  {
    static char pcTmp[16];
    G_TestDialog * pGTD;
    pGTD = (G_TestDialog *)pLis;
    pGTD->theMenuGZ->pLPosition->setText(ExaktG::toString(pos,pcTmp,16));
    pGTD->theMenuGZ->pLPosition->show(pGTD->Parent->theGUI->getMainSurface());
  }

  void G_TestDialog::aPosLis(void * pLis,float pos)
  {
    static char pcTmp[16];
    G_TestDialog * pGTD;
    pGTD = (G_TestDialog *)pLis;
    pGTD->theMenuGA->pLPosition->setText(ExaktG::toString(pos,pcTmp,16));
    pGTD->theMenuGA->pLPosition->show(pGTD->Parent->theGUI->getMainSurface());
  }

  void G_TestDialog::gFLis(void * pLis,int iA,int iB,int iC,int iD)
  {
    
  }


  static void G_TestDialogKeyListener(void * src, SDL_Event * evt)
  {
    G_TestDialog* ad = (G_TestDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    //    SDLMod mod = key->keysym.mod;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->escape_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->return_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_LEFT || key->keysym.sym == SDLK_F4)
	  {
	    ad->left_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RIGHT || key->keysym.sym == SDLK_F5)
	  {
	    ad->right_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEUP)
	  {
	    ad->GLiftUpListener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEDOWN)
	  {
	    ad->GLiftDownListener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_LEFT)
	  {
	    ad->GY_LeftListener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RIGHT)
	  {
	    ad->GY_RightListener(src,evt);
	  }
	/*	else if(key->keysym.sym == SDLK_p)
	  {
	    if((mod & KMOD_CTRL) &&		\
	       (mod & KMOD_SHIFT)&&             \
	       (mod & KMOD_ALT))
	      {
		printf("G_TestDialog Str Shift Alt p\n");
	      }
	      }*/
      }
    /*    if( key->type == SDL_KEYUP )
      {
	
      }*/
  }

  void G_TestDialog::incSpeedLevel(int axis)
  {
    MenuGBase * gMenu;
    int speedLevel;

    if(ExaktG::AxisX==axis){
      gMenu = theMenuGX;
    }
    else if(ExaktG::AxisY==axis){
      gMenu = theMenuGY;
    }
    else if(ExaktG::AxisZ==axis){
      gMenu = theMenuGZ;
    }
    else if(ExaktG::AxisA==axis){
      gMenu = theMenuGA;
    }
    this->pExaktG->incSpeedLevel(axis);
    speedLevel = this->pExaktG->getSpeedLevel(axis);
    gMenu->pLSpeed->setText(this->pExaktG->getSpeedText(axis,speedLevel));
    gMenu->pLSpeed->show(Parent->theGUI->getMainSurface());
  }

  G_TestDialog::G_TestDialog(int sdlw,		\
			 int sdlh,		\
			 int camw,		\
			 int camh,		\
			 int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    short Zeile1_y;
    //short Zeile3_y;
    short Zeile5_y;

    this->Parent = parent;
    this->pExaktG = this->Parent->getExaktG();
    this->pGCtrl = this->pExaktG->getG_Ctrl();

    this->tGCodeLis.pTheListener = this;
    this->tGCodeLis.fnkXPosLis =&this->xPosLis;
    this->tGCodeLis.fnkYPosLis =&this->yPosLis;
    this->tGCodeLis.fnkZPosLis =&this->zPosLis;
    this->tGCodeLis.fnkAPosLis =&this->aPosLis;
    this->tGCodeLis.fnkGFLis =&this->gFLis;

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

    MLinks_x = sdlw/2 - 504;//Breite von 1008 und mittig

    //vertikal fünf Zeilen
    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    //Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    //short Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    short Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    //horizontal acht Spalten
    int width = 1008;
    int x_space = 2;
    int Bw = (width -7*x_space)/8;
    int B1x = sdlw/2 - width/2;
    int B2x = B1x + 1*Bw+1*x_space;
    int B3x = B1x + 2*Bw+2*x_space;
    //int B4x = B1x + 3*Bw+3*x_space;
    int B5x = B1x + 4*Bw+4*x_space;
    //int B6x = B1x + 5*Bw+5*x_space;
    int B7x = B1x + 6*Bw+6*x_space;
    //int B8x = B1x + 7*Bw+7*x_space;

    theMenuBarSettings.Text[0]=0;
    theMenuBarSettings.Text[1]=(char *)"F2 status";
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=(char *)"F4 prev";
    theMenuBarSettings.Text[4]=(char *)"F5 next";
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=getstatus_listener;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=left_listener;
    theMenuBarSettings.evtFnks[4]=right_listener;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=escape_listener;
    theMenuBarSettings.evtFnks[7]=return_listener;

    theMenu = new MenuBar((int)MLinks_x,(int)Zeile5_y,(int)MZeile_h,(char*)"G-Test", \
			  &this->theMenuBarSettings,Parent);

    struct t_MenuGSettings * pGMset;

    pGMset = &theMenuGXSettings;
    pGMset->evtSource = this;
    pGMset->evtFnkBtn1Up = GX_LeftListener;
    pGMset->evtFnkBtn1Down = 0;//GX_LeftListener;
    pGMset->evtFnkBtn1MouseOver = GX_Btn1MouseOverListener;
    pGMset->evtFnkBtn2MouseOver = GX_Btn2MouseOverListener;
    pGMset->evtFnkBtn2Up = GX_RightListener;
    pGMset->evtFnkBtn2Down =0;//GX_RightListener;
    pGMset->evtFnkSetSpeed = GX_SpeedListener;
    pGMset->btn1Text = (char *)"<--a";
    pGMset->btn2Text = (char *)"d-->";
    pGMset->SpeedLabelText = pExaktG->getSpeedText(ExaktG::AxisX,0);
    
    pGMset = &theMenuGYSettings;
    pGMset->evtSource = this;
    pGMset->evtFnkBtn1Up = GY_LeftListener;
    pGMset->evtFnkBtn1Down = GY_LeftListener;
    pGMset->evtFnkBtn2Up = GY_RightListener;
    pGMset->evtFnkBtn2Down = 0;//GX_LeftListener;
    pGMset->evtFnkBtn1MouseOver = GY_Btn1MouseOverListener;
    pGMset->evtFnkBtn2MouseOver = GY_Btn2MouseOverListener;
    pGMset->evtFnkSetSpeed = GY_SpeedListener;
    pGMset->btn1Text = (char *)"Left";
    pGMset->btn2Text = (char *)"Right";
    pGMset->SpeedLabelText = pExaktG->getSpeedText(ExaktG::AxisY,0);

    pGMset = &theMenuGASettings;
    pGMset->evtSource = this;
    pGMset->evtFnkBtn1Up = GA_UpListener;
    pGMset->evtFnkBtn1Down = 0;//GA_LeftListener;
    pGMset->evtFnkBtn2Up = GA_DownListener;
    pGMset->evtFnkBtn2Down = 0;//GA_LeftListener;
    pGMset->evtFnkBtn1MouseOver = GA_Btn1MouseOverListener;
    pGMset->evtFnkBtn2MouseOver = GA_Btn2MouseOverListener;
    pGMset->evtFnkSetSpeed = GA_SpeedListener;
    pGMset->btn1Text = (char *)"Up";
    pGMset->btn2Text = (char *)"Down";
    pGMset->SpeedLabelText = pExaktG->getSpeedText(ExaktG::AxisA,0);

    pGMset = &theMenuGZSettings;
    pGMset->evtSource = this;
    pGMset->evtFnkBtn1Up = GLiftUpListener;
    pGMset->evtFnkBtn1Down = 0;//GLiftUpListener;
    pGMset->evtFnkBtn2Up = GLiftDownListener;
    pGMset->evtFnkBtn2Down = 0;//
    pGMset->evtFnkBtn1MouseOver = GZ_Btn1MouseOverListener;
    pGMset->evtFnkBtn2MouseOver = GZ_Btn2MouseOverListener;
    pGMset->evtFnkSetSpeed = GLiftSetSpeedListener;
    pGMset->btn1Text = (char *)"PgUp";
    pGMset->btn2Text = (char *)"PgDown";
    pGMset->SpeedLabelText = pExaktG->getSpeedText(ExaktG::AxisZ,0);

    theMenuGX = new MenuGVertical((char *)"X-Achse",			\
				    B1x,				\
				    (int)Zeile1_y,			\
				    MZeile_h,				\
				    MSpace_h,				\
				    2*Bw,				\
				    &this->theMenuGXSettings,Parent);

    theMenuGY = new MenuGVertical((char *)"Y-Achse",			\
				    B3x,				\
				    (int)Zeile1_y,			\
				    MZeile_h,				\
				    MSpace_h,				\
				    2*Bw,				\
				    &this->theMenuGYSettings,Parent);

    theMenuGA = new MenuGHorizontal((char *)"Walze",		\
				 B5x,					\
				 (int)Zeile1_y,				\
				 MZeile_h,				\
				 MSpace_h,				\
				 2*Bw,				\
				 &this->theMenuGASettings,Parent);

    theMenuGZ = new MenuGHorizontal((char *)"Lift",			\
				 B7x,					\
				 (int)Zeile1_y,				\
				 MZeile_h,				\
				 MSpace_h,				\
				 2*Bw,				\
				 &this->theMenuGZSettings,Parent);

    pBEingabe = new Button("Enter",					\
			   B1x,						\
			   (int)Zeile4_y,				\
			   Bw,					\
			   MZeile_h,					\
			   Parent->MenuSet);
    pBEingabe->setLMButtonUpEvtHandler(return_listener);
    pBEingabe->pTSource = this;

    pTFEingabe = new TextField(0,			\
			       /*TF_MaxLen*/64,				\
			       B2x,					\
			       (int)Zeile4_y,				\
			       3*Bw,					\
			       MZeile_h,				\
			       Parent->WerteSet);
    pTFEingabe->setActive(true);

    //addEvtTarget(Label_Value);
    theMenu->addToEvtTarget(this);

    theMenuGX->addToEvtTarget(this);
    theMenuGY->addToEvtTarget(this);
    theMenuGA->addToEvtTarget(this);
    theMenuGZ->addToEvtTarget(this);
    this->addEvtTarget(pBEingabe);
    this->addEvtTarget(pTFEingabe);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(G_TestDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
  }

  void G_TestDialog::setActive(void)
  {
    this->pExaktG->setGCodeResultListener(&this->tGCodeLis);
    this->pGCtrl->cmdGetStatus();
  }
}/* end Namespace */
