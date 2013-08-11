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
#include "ErrorDialog.h"
#include "Dialoge.h"

namespace EuMax01
{

  void CalibrationDialog::escape_listener(void * src, SDL_Event * evt)
  {
    CalibrationDialog* ad = (CalibrationDialog*)src;//KeyListener
    ad->Parent->showArbeitsDialog();
  }

  void CalibrationDialog::left_listener(void * src, SDL_Event * evt)
  {
    CalibrationDialog* ad = (CalibrationDialog*)src;//KeyListener
    ad->decStep();
  }

  void CalibrationDialog::right_listener(void * src, SDL_Event * evt)
  {
     CalibrationDialog* ad = (CalibrationDialog*)src;//KeyListener
     ad->incStep();
  }
  
  void CalibrationDialog::return_listener(void * src, SDL_Event * evt)
  {
    CalibrationDialog* ad = (CalibrationDialog*)src;//KeyListener
    ad->resetStepValue();
    ad->incStep();
  }

  static void CalibrateDialogKeyListener(void * src, SDL_Event * evt)
  {
    CalibrationDialog* ad = (CalibrationDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    SDLMod mod = key->keysym.mod;

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
	else if(key->keysym.sym == SDLK_p)
	  {
	    if((mod & KMOD_CTRL) &&		\
	       (mod & KMOD_SHIFT)&&             \
	       (mod & KMOD_ALT))
	      {
		ad->bPaintRequest = true;
		ad->Parent->showCalibrationDialog();
		printf("set paintRequest\n");
	      }
	  }
      }
  }

  CalibrationDialog::CalibrationDialog(int sdlw,		\
			 int sdlh,		\
			 int camw,		\
			 int camh,		\
			 int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y;

    this->Parent = parent;
    this->ActualStep = CalibrationDialog::iQ1;

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
			   MLinks_x,Zeile1_y,506*2,MZeile_h,Parent->MenuSet);
    Label_Step->setFontColor(&Globals::GlobalSDL_Color3);//ein rot

    Label_ValueName = new Label(this->ValueName,			\
				MLinks_x,Zeile3_y,			\
				506-MSpace_h,MZeile_h);  
    Label_ValueName->setText(Parent->TextCam1Xaxis);

    snprintf(this->Value,64,"---");
    Label_Value = new Label(Value,					\
			    MLinks_x+506+MSpace_h,			\
			    Zeile3_y,506-MSpace_h,			\
			    MZeile_h,Parent->WerteSet);
    Label_Value->setBorder(true);

    theMenuBarSettings.Text[0]=0;
    theMenuBarSettings.Text[1]=0;
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=(char *)"F4 prev";
    theMenuBarSettings.Text[4]=(char *)"F5 next";
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=0;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=left_listener;
    theMenuBarSettings.evtFnks[4]=right_listener;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=escape_listener;
    theMenuBarSettings.evtFnks[7]=return_listener;

    theMenu = new MenuBar((int)MLinks_x,(int)Zeile5_y,(int)MZeile_h,(char*)"Calibration", \
			  &this->theMenuBarSettings,Parent);

    addEvtTarget(Label_Step);
    addEvtTarget(Label_ValueName);
    addEvtTarget(Label_Value);
    theMenu->addToEvtTarget(this);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(CalibrateDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
  }

  void CalibrationDialog::resetStepValue()
  {
    switch(this->ActualStep)
      {
      case CalibrationDialog::iQ1:
	{
	  this->Parent->sendProtocolMsg(nPEC_RESET_Q1);
	  this->Parent->sendProtocolMsg(nPEC_GET_Q1);
	  break;
	}
      case CalibrationDialog::iQ2:
	{
	  this->Parent->sendProtocolMsg(nPEC_RESET_Q2);
	  this->Parent->sendProtocolMsg(nPEC_GET_Q2);
	  break;
	}
      case CalibrationDialog::iZ1:
	{
	  this->Parent->sendProtocolMsg(nPEC_RESET_Z1);
	  this->Parent->sendProtocolMsg(nPEC_GET_Z1);
	  break;
	}
      case CalibrationDialog::iZ2:
	{
	  this->Parent->sendProtocolMsg(nPEC_RESET_Z2);
	  this->Parent->sendProtocolMsg(nPEC_GET_Z2);
	  break;
	}
      }
  }

  void CalibrationDialog::incStep()
  {
    ActualStep++;
    if(ActualStep>CalibrationDialog::iZ1)
      {
	ActualStep = CalibrationDialog::iQ1;//reset fürs nächste mal
	this->showStep(this->ActualStep);
	Parent->showArbeitsDialog();
      }
    else
      {
	this->showStep(this->ActualStep);
      }
  }
  
  void CalibrationDialog::decStep()
  {
    if(ActualStep>0)
      {
	ActualStep--;
      }
    this->showStep(this->ActualStep);
  }

  void CalibrationDialog::showStep(int theStep)
  {
    //Wert Anzeige resetten
    snprintf(this->Value,64,"---");
    this->Label_Value->setText(this->Value);
    this->Label_Value->show(this->Parent->theGUI->getMainSurface());

    switch(theStep)
      {
      case CalibrationDialog::iQ1:
	{
	  this->ActualStep = theStep;

	  snprintf(this->StepText,256,				\
		   "Move camera 1 X-Axis in zero position :");
	  this->Label_Step->setText(this->StepText);

	  this->Label_ValueName->setText(Parent->TextCam1Xaxis);
	  this->Parent->sendProtocolMsg(nPEC_GET_Q1);
	  break;
	}
      case CalibrationDialog::iQ2:
	{
	  this->ActualStep = theStep;

	  snprintf(this->StepText,256,				\
		   "Move camera 2 X-Axis in zero position :");
	  this->Label_Step->setText(this->StepText);


	  this->Label_ValueName->setText(Parent->TextCam2Xaxis);
	  this->Parent->sendProtocolMsg(nPEC_GET_Q2);
	  break;
	}
      case CalibrationDialog::iZ1:
	{
	  this->ActualStep = theStep;

	  snprintf(this->StepText,256,				\
		   "Move Z-Axis in zero position :");
	  this->Label_Step->setText(this->StepText);

	  this->Label_ValueName->setText(Parent->TextZaxis);
	  this->Parent->sendProtocolMsg(nPEC_GET_Z1);
	  break;
	}
      case CalibrationDialog::iZ2:
	{
	  this->ActualStep = theStep;

	  snprintf(this->StepText,256,				\
		   "Move camera 2 Z-Axis in zero position :");
	  this->Label_Step->setText(this->StepText);


	  snprintf(this->ValueName,16,"Z2");
	  this->Label_ValueName->setText(this->ValueName);
	  this->Parent->sendProtocolMsg(nPEC_GET_Z2);
	  break;
	}
      }
    this->Label_Step->show(this->Parent->theGUI->getMainSurface());
    this->Label_ValueName->show(this->Parent->theGUI->getMainSurface());
  }
  
  void CalibrationDialog::setQ1(unsigned int dat)
  {
    setXXData(dat,CalibrationDialog::iQ1,(char*)" mm");  
  }

  void CalibrationDialog::setQ2(unsigned int dat)
  {
    setXXData(dat,CalibrationDialog::iQ2,(char*)" mm");
  }

  void CalibrationDialog::setZ1(unsigned int dat,char*suffix)
  {
    setXXData(dat,CalibrationDialog::iZ1,suffix);    
  }

  void CalibrationDialog::setZ2(unsigned int dat,char*suffix)
  {
    setXXData(dat,CalibrationDialog::iZ2,suffix);
  }

  void CalibrationDialog::setXXData(unsigned int dat,int MyStep,char*suffix)
  {
    float showValue;

    if(MyStep==this->ActualStep)
      {
	showValue = this->Parent->convertMBProtData(dat);
	sprintf(this->Value,"%7.2f%s",showValue,suffix);
	this->Label_Value->setText(this->Value);
	this->Label_Value->show(this->Parent->theGUI->getMainSurface());
      }
  }

  ForceCalDialog::ForceCalDialog(int sdlw,		\
				 int sdlh,		\
				 int camw,			\
				 int camh,				\
				 int yPos,ArbeitsDialog * parent)	\
    :CalibrationDialog(sdlw,sdlh,camw,camh,yPos,parent)
  {
    pLastActiveScreen = 0;
    bQ1OverflowPending = false;
    bQ2OverflowPending = false;
    bZ1OverflowPending = false;
    bZ2OverflowPending = false;
  }

  void ForceCalDialog::showForceCalDialog(int problemSource,Screen* pLastActiveScreen)
  {
    //Parent iActiveDialog sichern, damit der Zustand später wieder hergestellt werden kann
    //Parent iActiveDialog = ForceCalDialog setzten, und damit die AchsenEvents umleiten
    //Kommen nun weitere "SyncLostEvents" also zu schnell an den Achsen gedreht, dann muss das Ereignis
    //mit einem Flag festgehalten werden, sofern es sich nicht um die aktuell behandelte Achse handelt, 
    //damit diese "neue" Achse danach auch behandelt werden kann.
    //

    //if(Parent->iActiveDialog != ArbeitsDialog::ForceCalDialog)
    //   this->last_ActiveScreen = pLastActiveScreen; //Das ist der Screen bevor das Overflow Evt kam
    //   this->last_iActiveDialog = Parent->iActiveDialog;
    //   Parent->iActiveDialog = ArbeitsDialog::ForceCalDialog;
    //else //wir sind schon in ForceCalDialog, und es kommt ein weiteres Event, was nur von einer weiteren Quelle 
    //kommen kann
    //  Andere ProblemSourcen als die Aktuelle merken
    switch(problemSource)
      {
      case iQ1:
	{
	  bQ1OverflowPending = true;
	}
      case iQ2:
	{
	  bQ2OverflowPending = true;
	}
      case iZ1:
	{
	  bZ1OverflowPending = true;
	}
      case iZ2:
	{
	  bZ2OverflowPending = true;
	}
      }
  }

  PosDialog::~PosDialog(){}
  PosDialog::PosDialog(char* text,		\
		       int sdlw,		\
		       int sdlh,		\
		       int camw,		\
		       int camh,		\
		       int yPos,\
		       ArbeitsDialog * parent)
  {
    short M_y;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Y1,Y2,Y3,Y4;
    short B1x, B2x, B3x, B4x;
    unsigned short Bw , x_space;

    //unsigned short HSpace,VSpace;
    //unsigned short LineH;
    int i;
    this->Parent = parent;
    for(i=0;i<64;i++)
      {
	if(i<=3)
	  {
	    pcLabelCam1[0][i] = '-';
	    pcLabelCam1[1][i] = '-';
	    pcLabelCam1[2][i] = '-';
	    pcLabelCam2[0][i] = '-';
	    pcLabelCam2[1][i] = '-';
	    pcLabelCam2[2][i] = '-';
	    pcLabelZ[0][i] = '-';
	    pcLabelZ[1][i] = '-';
	    pcLabelZ[2][i] = '-';
	  }
	else
	  {
	    pcLabelCam1[0][i] = '\0';
	    pcLabelCam1[1][i] = '\0';
	    pcLabelCam1[2][i] = '\0';
	    pcLabelCam2[0][i] = '\0';
	    pcLabelCam2[1][i] = '\0';
	    pcLabelCam2[2][i] = '\0';
	    pcLabelZ[0][i] = '\0';
	    pcLabelZ[1][i] = '\0';
	    pcLabelZ[2][i] = '\0';
	  }
      }

    DiffFarben[0]=Globals::GlobalUint32ColorE1;
    DiffFarben[1]=Globals::GlobalUint32ColorE2;
    DiffFarben[2]=Globals::GlobalUint32ColorE3;
    DiffFarben[3]=Globals::GlobalUint32ColorE4;
    DiffFarben[4]=Globals::GlobalUint32ColorE5;
    DiffFarben[5]=Globals::GlobalUint32ColorE6;
    DiffFarben[6]=Globals::GlobalUint32ColorE7;
    DiffFarben[7]=Globals::GlobalUint32ColorE8;
    DiffFarben[8]=Globals::GlobalUint32ColorE9;
    DiffFarben[9]=Globals::GlobalUint32ColorE10;
    DiffFarben[10]=Globals::GlobalUint32ColorE11;
    DiffFarben[11]=Globals::GlobalUint32ColorE12;
    DiffFarben[12]=Globals::GlobalUint32ColorE13;
    DiffFarben[13]=Globals::GlobalUint32ColorE14;
    DiffFarben[14]=Globals::GlobalUint32ColorE15;
    DiffFarben[15]=Globals::GlobalUint32ColorE16;

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
    /*  HSpace = 3;
    VSpace = 3;
    Y1  = sdlh - yPos;
    //Y1  = y+VSpace;
    //Aw = w/5 - 1*HSpace;
    //Bw = (w-(Aw+4*HSpace))/3;
    Bw = (w-4*HSpace)/4;
    B1x = x + 1*HSpace + 0*Bw;
    B2x = x + 2*HSpace + 1*Bw;
    B3x = x + 3*HSpace + 2*Bw;
    B4x = x + 4*HSpace + 3*Bw;

    LineH = (h - 4*VSpace)/4;
    
    Y2 = Y1 + LineH + VSpace;
    Y3 = Y2 + LineH + VSpace;
    Y4 = Y3 + LineH + VSpace;*/

    Y1 = yPos + 1*MSpace_h + 0*MZeile_h;
    Y2 = yPos + 2*MSpace_h + 1*MZeile_h;
    Y3 = yPos + 3*MSpace_h + 2*MZeile_h;
    Y4 = yPos + 4*MSpace_h + 3*MZeile_h;

    x_space = 5;
    Bw = (1012-3*x_space)/4;

    B1x = sdlw/2 - 506;
    B2x = B1x + 1*Bw+1*x_space;
    B3x = B1x + 2*Bw+2*x_space;
    B4x = B1x + 3*Bw+3*x_space;

    LabelRecipeName = new Label("",B1x,Y1,Bw,MZeile_h,Parent->DialogSet);

    LabelStep = new Label("Step 1",B4x,Y1,Bw,MZeile_h,Parent->DialogSet);

    //difference
    LabelActual = new Label("Current Position",B2x,Y1,Bw,MZeile_h,Parent->DialogSet);

    pLabelCam1[PosDialog::iLabelName] = new Label(Parent->TextCam1Xaxis,\
						  B1x,Y2,Bw,MZeile_h);
    pLabelCam2[PosDialog::iLabelName] = new Label(Parent->TextCam2Xaxis,\
						  B1x,Y3,Bw,MZeile_h);
    pLabelZ[PosDialog::iLabelName] = new Label(Parent->TextZaxis,\
					       B1x,Y4,Bw,MZeile_h);

    pLabelCam1[PosDialog::iStep] = new Label(pcLabelCam1[PosDialog::iStep],\
					     B4x,Y2,Bw,MZeile_h);
    pLabelCam2[PosDialog::iStep] = new Label(pcLabelCam2[PosDialog::iStep],\
					     B4x,Y3,Bw,MZeile_h);
    pLabelZ[PosDialog::iStep] = new Label(pcLabelZ[PosDialog::iStep],\
					  B4x,Y4,Bw,MZeile_h);

    pLabelCam1[PosDialog::iDiff] = new Label(pcLabelCam1[PosDialog::iDiff],\
					     B3x,Y2,Bw,MZeile_h);
    pLabelCam2[PosDialog::iDiff] = new Label(pcLabelCam2[PosDialog::iDiff],\
					     B3x,Y3,Bw,MZeile_h);
    pLabelZ[PosDialog::iDiff] = new Label(pcLabelZ[PosDialog::iDiff],\
					  B3x,Y4,Bw,MZeile_h);

    pLabelCam1[PosDialog::iDiff]->setFont(Globals::getFontButtonBig());
    pLabelCam2[PosDialog::iDiff]->setFont(Globals::getFontButtonBig());
    pLabelZ[PosDialog::iDiff]->setFont(Globals::getFontButtonBig());
    pLabelCam1[PosDialog::iDiff]->setFontColor(&Globals::GlobalSDL_Color2);
    pLabelCam2[PosDialog::iDiff]->setFontColor(&Globals::GlobalSDL_Color2);
    pLabelZ[PosDialog::iDiff]->setFontColor(&Globals::GlobalSDL_Color2);


    pLabelCam1[PosDialog::iCurr] = new Label(pcLabelCam1[PosDialog::iCurr],\
					     B2x,Y2,Bw,MZeile_h);
    pLabelCam2[PosDialog::iCurr] = new Label(pcLabelCam2[PosDialog::iCurr],\
					     B2x,Y3,Bw,MZeile_h);
    pLabelZ[PosDialog::iCurr] = new Label(pcLabelZ[PosDialog::iCurr],\
					  B2x,Y4,Bw,MZeile_h);

    for(int i=0;i<3;i++)
      {
	if(i!=PosDialog::iDiff)
	  {
	    pLabelCam1[i]->setNormalColor(Parent->WerteSet->getNormalColor());
	    pLabelCam1[i]->setMarkedColor(Parent->WerteSet->getMarkedColor());
	    pLabelCam1[i]->setFontColor(Parent->WerteSet->getFontColor());
	    pLabelCam1[i]->setFont(Parent->WerteSet->getFont());

	    pLabelCam2[i]->setNormalColor(Parent->WerteSet->getNormalColor());
	    pLabelCam2[i]->setMarkedColor(Parent->WerteSet->getMarkedColor());
	    pLabelCam2[i]->setFontColor(Parent->WerteSet->getFontColor());
	    pLabelCam2[i]->setFont(Parent->WerteSet->getFont());

	    pLabelZ[i]->setNormalColor(Parent->WerteSet->getNormalColor());
	    pLabelZ[i]->setMarkedColor(Parent->WerteSet->getMarkedColor());
	    pLabelZ[i]->setFontColor(Parent->WerteSet->getFontColor());
	    pLabelZ[i]->setFont(Parent->WerteSet->getFont());
	  }
      }


    addEvtTarget(LabelRecipeName);
    addEvtTarget(LabelStep);
    addEvtTarget(LabelActual);

    for(int i = 0; i<4 ;i++)
      {
	addEvtTarget(pLabelCam1[i]);
	addEvtTarget(pLabelCam2[i]);
	addEvtTarget(pLabelZ[i]);
      }
  };

  void PosDialog::showDifferenceCam1(int aktuell,int rezept)
  {
    unsigned int e = 0;
    e=printDifference(pcLabelCam1[PosDialog::iDiff],aktuell,rezept);
    pLabelCam1[PosDialog::iDiff]->setMarkedColor(DiffFarben[e]);
    pLabelCam1[PosDialog::iDiff]->setText(pcLabelCam1[PosDialog::iDiff]);
    pLabelCam1[PosDialog::iDiff]->show(Parent->theGUI->getMainSurface());
  }
  
  void PosDialog::showDifferenceCam2(int aktuell,int rezept)
  {
    unsigned int e =0;
    e=printDifference(pcLabelCam2[PosDialog::iDiff],aktuell,rezept);
    pLabelCam2[PosDialog::iDiff]->setMarkedColor(DiffFarben[e]);
    pLabelCam2[PosDialog::iDiff]->setText(pcLabelCam2[PosDialog::iDiff]);
    pLabelCam2[PosDialog::iDiff]->show(Parent->theGUI->getMainSurface());
  }
  
  void PosDialog::showDifferenceZ(int aktuell,int rezept)
  {
    unsigned int e = 0;
    e=printDifference(pcLabelZ[PosDialog::iDiff],aktuell,rezept);
    pLabelZ[PosDialog::iDiff]->setMarkedColor(DiffFarben[e]);
    pLabelZ[PosDialog::iDiff]->setText(pcLabelZ[PosDialog::iDiff]);
    pLabelZ[PosDialog::iDiff]->show(Parent->theGUI->getMainSurface());
  }

  unsigned int PosDialog::printDifference(char * target,int aktuell,int rezept)
  {
    int distanz=0;
    unsigned int GradDerEntfernung = 0;
    char FuellZeichen = ' ';
    for(unsigned int i = 0;i<PosDialog::DiffLen;i++)
      {
	target[i] = '\0';
      }
    target[PosDialog::DiffLen] = '\0';
    
    if(aktuell<rezept)//links von ref
      {
	FuellZeichen = PosDialog::DiffZeichenLinks;
	distanz = rezept-aktuell;
      }
    else if(aktuell>rezept)//rechts von ref
      {
	FuellZeichen = PosDialog::DiffZeichenRechts;
	distanz = aktuell-rezept;
      }
    else//Volltreffer!
      {
	FuellZeichen = PosDialog::DiffZeichenMitte;
	distanz = 0;
      }

    for(unsigned int i =0;i<PosDialog::DiffLen;i++) //Wie weit ist die Zielposition entfernt?
      {
	if((distanz/=2)>0)
	  {
	    target[i]=FuellZeichen;
	    GradDerEntfernung++;
	  }
	else
	  {
	    break;
	  }
      }
    return GradDerEntfernung;
  }

  void PosDialog::showRecipeName(char * text)
  {
    snprintf(pcLabelRecipeName,64,"Name:%s",text);
    LabelRecipeName->setText(pcLabelRecipeName);
    LabelRecipeName->show(Parent->theGUI->getMainSurface());
  }

  void LoadDialog::escape_listener(void * src, SDL_Event * evt)
  {
    LoadDialog* ad = (LoadDialog*)src;//KeyListener
    ad->Parent->showArbeitsDialog();
  }

  void LoadDialog::filter_listener(void * src, SDL_Event * evt)
  {
    LoadDialog* ad = (LoadDialog*)src;//KeyListener
    ad->Parent->showLoadDialog(0,ad->LoadMode);
  }

  void LoadDialog::pgup_listener(void * src, SDL_Event * evt)
  {
    LoadDialog* ad = (LoadDialog*)src;//KeyListener
    ad->naviPageup();
  }

  void LoadDialog::pgdown_listener(void * src, SDL_Event * evt)
  {
    LoadDialog* ad = (LoadDialog*)src;//KeyListener
    ad->naviPagedown();
  }

  void LoadDialog::return_listener(void * src, SDL_Event * evt)
  {
    LoadDialog* ad = (LoadDialog*)src;//KeyListener
    ad->naviReturn();
  }

  static void LoadDialogKeyListener(void * src, SDL_Event * evt)
  {
    LoadDialog* ad = (LoadDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->escape_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F2)
	  {
	    ad->filter_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_UP)
	  {
	    ad->naviUp();
	  }
	else if(key->keysym.sym == SDLK_DOWN)
	  {
	    ad->naviDown();
	  }
	else if(key->keysym.sym == SDLK_RIGHT)
	  {
	    ad->naviRight();
	  }
	else if(key->keysym.sym == SDLK_LEFT)
	  {
	    ad->naviLeft();
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->return_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEDOWN || key->keysym.sym == SDLK_F6)
	  {
	    ad->pgdown_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEUP || key->keysym.sym == SDLK_F5)
	  {
	    ad->pgup_listener(src,evt);
	  }
      }
  }
  
  static void LabelMouseUpListener(void * src, SDL_Event * evt)
  {
    struct t_LoadDialogButtonEventContainer * lc;
    LoadDialog * ld;
    lc =(struct t_LoadDialogButtonEventContainer *)src;
    ld = (LoadDialog *)lc->theLoadDialog;
    
    if(lc->index==ld->getActiveRecipe())
      {
	ld->naviReturn();
	return;
      }
    
    if(lc->index < ld->getMaxRecipesToDisplay())
      {
	ld->setActiveRecipe(lc->index);
      }
  }

  static TextField * pTF_Filter4dirFilter=0;

  LoadDialog::LoadDialog(int sdlw,		\
			 int sdlh,		\
			 int camw,		\
			 int camh,		\
			 int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    unsigned short Rezepte_y;
    short Rezepte_w;
    short MLoadName_y;
    short x_space;

    this->Parent = parent;
    this->ActiveRecipe = 0;
    this->MaxRecipesToDisplay = 0;
    this->ActiveSavePage = 0;
    this->MaxSavePages = 0;
    this->tmpRezept = new Rezept();
    this->LoadMode = false;

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

    x_space = 2;
    MLinks_x = sdlw/2 - 506;

    MLoadName_y  = yPos + 5*MSpace_h + 4*MZeile_h;
    Rezepte_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Rezepte_w = 125;

    theMenuBarSettings.Text[0]=0;
    theMenuBarSettings.Text[1]=(char *)"F2 filter";
    theMenuBarSettings.Text[2]=0;//Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    theMenuBarSettings.Text[3]=0;//Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    theMenuBarSettings.Text[4]=(char *)"F5 prev";
    theMenuBarSettings.Text[5]=(char *)"F6 next";
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=filter_listener;
    theMenuBarSettings.evtFnks[2]=0;//Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    theMenuBarSettings.evtFnks[3]=0;//Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    theMenuBarSettings.evtFnks[4]=pgup_listener;
    theMenuBarSettings.evtFnks[5]=pgdown_listener;
    theMenuBarSettings.evtFnks[6]=escape_listener;
    theMenuBarSettings.evtFnks[7]=return_listener;

    theMenu = new LoadMenuBar((int)MLinks_x,(int)MLoadName_y,(int)MZeile_h, \
			      (char*)"Load",				\
			      &this->theMenuBarSettings,Parent,\
			      LoadDialog::MaxRezeptFileLaenge);

    unsigned int ii = 0;
    //char tmpc[16] = { ' '};
    //tmpc[15]='\0';
    blankText = (char *)" ";
    Globals* global = Globals::getInstance();
    for(unsigned int i=0;i<LoadDialog::RezepteLen;i++)
      {
	if(ii>=LoadDialog::RezepteProZeile)
	  {
	    ii=0;
	    Rezepte_y += 1*MSpace_h + 1*MZeile_h;
	  }
	//	sprintf(DateiNamen[i],"%s",tmpc);                   //Text Buffer füllen
	pLabel_Rezepte[i] = new Label(blankText,			\
				      MLinks_x + ii*x_space + ii*Rezepte_w, \
				      Rezepte_y,			\
				      Rezepte_w,			\
				      MZeile_h,				\
				      Parent->WerteSet);
	pLabel_Rezepte[i]->setFont(global->getDefaultFont());
	LabelEvtContainers[i].theLoadDialog = (void *)this;
	LabelEvtContainers[i].index = i;
	pLabel_Rezepte[i]->setLMButtonUpEvtHandler(LabelMouseUpListener,(void*)&LabelEvtContainers[i]);
	ii++;
	this->addEvtTarget(pLabel_Rezepte[i]);
      }

    pTF_Filter4dirFilter = theMenu->TextField_Filter;

    this->setLoadMode(true);

    this->pTSource = this;
    this->setKeyboardUpEvtHandler(LoadDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    theMenu->addToEvtTarget(this);
  }

  unsigned int LoadDialog::getMaxRecipesToDisplay()
  {
    return this->MaxRecipesToDisplay;
  }

  unsigned int LoadDialog::getActiveRecipe()
  {
    return this->ActiveRecipe;
  }

  void LoadDialog::setLoadMode(bool loadMode)
  {
    this->LoadMode=loadMode;
    if(LoadMode)
      {
	theMenu->setMenuName((char*)"Load");
      }
    else
      {
	theMenu->setMenuName((char*)"Delete");
      }
    /*  TODO  if(this->LoadMode!=loadMode)
      {
	this->LoadMode = 
	}*/
  }

  /* \brief Makiert das aktive Label und demaskiert den Vorherigen.
   */ 
  void LoadDialog::setActiveRecipe(unsigned int nr)
  {
    pLabel_Rezepte[ActiveRecipe]->setBorder(false);
    pLabel_Rezepte[ActiveRecipe]->repaint();
    
    if(nr<0)                     //Begrenzungen
      nr=0;
    if(nr>=LoadDialog::RezepteLen)
      nr=LoadDialog::RezepteLen-1;
    if(nr>=this->MaxRecipesToDisplay)
      nr=this->MaxRecipesToDisplay-1;
    if(nr<0)
      {
	this->show(this->Parent->theGUI->getMainSurface());
	return;
      }

    ActiveRecipe = nr;
    pLabel_Rezepte[ActiveRecipe]->setBorder(true);
    pLabel_Rezepte[ActiveRecipe]->repaint();
    this->show(this->Parent->theGUI->getMainSurface());
  }

  void LoadDialog::addToActiveRecipe(int summand)
  {
    int nr = this->ActiveRecipe;
    nr += summand;
    this->setActiveRecipe(nr);
  }

  int dirFilter(const struct dirent * dir)
  {
    int ret = 0;

    if(strlen(dir->d_name)>LoadDialog::MaxRezeptFileLaenge)
      {
	goto out_null;
      }
    if(strlen(dir->d_name)==1)
      {
	goto out_null;
      }
    if(strlen(dir->d_name)==2)
      {
	goto out_null;
      }
    if(strlen(pTF_Filter4dirFilter->getText())>0)
      {
	if(0==strncmp(dir->d_name,				\
		      pTF_Filter4dirFilter->getText(),		\
		      strlen(pTF_Filter4dirFilter->getText())))
	  {
	    goto out_eins;
	  }
	else
	  {
	    goto out_null;
	  }
      }
    else
      {
	goto out_eins;
      }

  out_eins:
    ret = 1;
  out_null:

    return ret;
  }

  void LoadDialog::clearFilter(void)
  {
    pTF_Filter4dirFilter->setText((char*)"");
  }

  int LoadDialog::readSaveDirectory(char * dirName,unsigned int page)
  {
    struct dirent **namelist;
    int n;
    int fileToShow;

    //this->EvtTargets.Next = 0;     //alle FileLabels deaktivieren
    //this->Next = 0;                //alles nach dem Keylistener = 0
    //this->addEvtTarget(this);      //den Screen Key Listener bei sich selber anmelden!
    /*    this->Label_LadenName->Next = 0; //Laden Label anzeigen
    this->addEvtTarget(Label_LadenName);
    this->Label_MenuText->Next = 0; //Laden MenuText anzeigen
    this->addEvtTarget(Label_MenuText);
    */
    //theMenu->addToEvtTarget(this);

    this->ActiveSavePage = page;

    n = scandir(dirName, &namelist, dirFilter, alphasort);

    if (n < 0)
      return -1;
    if (0==n)
      return 1;

    unsigned int ii = n;
    this->MaxSavePages = 0;
    while(ii>=LoadDialog::RezepteLen)
      {
	this->MaxSavePages++;
	ii-=LoadDialog::RezepteLen;
      }

    for(unsigned int i=0;i<LoadDialog::RezepteLen;i++)
      {
	pLabel_Rezepte[i]->setText(blankText);
      }

    fileToShow = page * LoadDialog::RezepteLen;
    if(fileToShow<=n)
      {
	//int tmp = 0;
	int i = 0;
	unsigned int i2 = 0;
	for(i=fileToShow,i2=0;i<n&&i2<LoadDialog::RezepteLen;i++,i2++)
	  {
	    strncpy(DateiNamen[i2],namelist[i]->d_name,LoadDialog::MaxRezeptFileLaenge);
	    pLabel_Rezepte[i2]->setText(DateiNamen[i2]); //neuen Text setzen
	    //pLabel_Rezepte[i2]->Next = 0;  // alles hinter diesem Label = 0
	    //this->addEvtTarget(pLabel_Rezepte[i2]);
	    MaxRecipesToDisplay = i2+1;
	  }
      }

    while(n--)
      {
	free(namelist[n]);
      }
    free(namelist);
    setActiveRecipe(0);
    return 0;
  }

  void LoadDialog::naviUp(){this->addToActiveRecipe(-LoadDialog::RezepteProZeile);}
  void LoadDialog::naviDown(){this->addToActiveRecipe(+LoadDialog::RezepteProZeile);} 
  void LoadDialog::naviLeft(){this->addToActiveRecipe(-1);}
  void LoadDialog::naviRight(){this->addToActiveRecipe(+1);}
  void LoadDialog::naviReturn()
  {
    char tmpBuf[1024];
    if(this->LoadMode)
      {
	if(!tmpRezept->readFromFile(Parent->pcSaveFilePath,DateiNamen[ActiveRecipe]))
	  {
	    Rezept::copy(tmpRezept,Parent->theRezept);
	    //Rezept::copy();
	    Parent->showRezept(0);
	    Parent->showArbeitsDialog();
	    //printf("ArbeitsDialog Update Recipe Daten\n");
	  }
	else
	  {
	    this->Parent->showErrorDialog((char*)"Error Reading SaveFile");
	  }
      }
    else //delete Mode
      {
	snprintf(tmpBuf,1024,"%s%s",\
		 Parent->pcSaveFilePath,\
		 DateiNamen[ActiveRecipe]);
	if(remove(tmpBuf))
	  {
	    printf( "Error deleting file: %s\n", strerror( errno ) );
	    this->Parent->showErrorDialog((char*)"Error Deleting Recipe File");
	  }
	else
	  {
	    this->Parent->showLoadDialog(0,false);
	  }
      }
  }

  void LoadDialog::naviPageup()
  {
    if(this->ActiveSavePage>=1)
      this->Parent->showLoadDialog(this->ActiveSavePage-1,this->LoadMode);
  }

  void LoadDialog::naviPagedown()
  {
    if(this->ActiveSavePage<this->MaxSavePages)
      this->Parent->showLoadDialog(this->ActiveSavePage+1,this->LoadMode);
  }

  static void NewCrossEscape(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    ad->decStep();
    ad->incStep();//Anzeige "durchspülen"
    
    ad->theMenuModus=NewDialog::iMainMenu;
    ad->setMenuBarForNewMain();
    //ad->Label_Menu->setText(NewDialogMainMenuText);
    //ad->Label_Menu->show(ad->Parent->theGUI->getMainSurface());
  }
  static void NewCrossEnter(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    ad->theMenuModus=NewDialog::iMainMenu;
    ad->setMenuBarForNewMain();
    //ad->Label_Menu->setText(NewDialogMainMenuText);
    //ad->Label_Menu->show(ad->Parent->theGUI->getMainSurface());
    
    ad->copyValuesToRezept();
    if(ad->getStep()<8)
      ad->incStep();
  }
  static void NewCrossCircleMinus(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    int step = -16;
    int dia = 0;
    SDLMod mod = SDL_GetModState();

    if((mod & KMOD_RSHIFT) || (mod & KMOD_LSHIFT))
      step=-4;
    dia = (*ad->Parent->getCircleDiameterFnk)();
    dia += step;
    (*ad->Parent->newCircleDiameterFnk)(dia);
    dia = (*ad->Parent->getCircleDiameterFnk)();
    cap_cam_setCrossXLimit(0,dia/2);
    cap_cam_setCrossXLimit(1,dia/2);

    dia = cap_cam_getCrossX(0);
    cap_cam_setCrossX(0,dia);
    dia = cap_cam_getCrossX(1);
    cap_cam_setCrossX(1,dia);
  }
  static void NewCrossCirclePlus(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    int step = 16;
    int dia = 0;
    SDLMod mod = SDL_GetModState();

    if((mod & KMOD_RSHIFT) || (mod & KMOD_LSHIFT))
      step=4;
    dia = (*ad->Parent->getCircleDiameterFnk)();
    dia += step;
    (*ad->Parent->newCircleDiameterFnk)(dia);
    dia = (*ad->Parent->getCircleDiameterFnk)();
    cap_cam_setCrossXLimit(0,dia/2);
    cap_cam_setCrossXLimit(1,dia/2);

    dia = cap_cam_getCrossX(0);
    cap_cam_setCrossX(0,dia);
    dia = cap_cam_getCrossX(1);
    cap_cam_setCrossX(1,dia);
  }
  static void NewCrossCam0Left(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    int step = -2;
    SDLMod mod = SDL_GetModState();
    if( (mod & KMOD_RSHIFT) || (mod & KMOD_LSHIFT) )
	step=-14;
    cap_cam_addCrossX(0,step);
    ad->getCam1CrossX();
  }
  static void NewCrossCam0Right(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    int step = 2;
    SDLMod mod = SDL_GetModState();
    if((mod & KMOD_RSHIFT) || (mod & KMOD_LSHIFT))
      step=14;
    cap_cam_addCrossX(0,step);
    ad->getCam1CrossX();
  }
  static void NewCrossCam1Left(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    int step = -2;
    SDLMod mod = SDL_GetModState();
    if( (mod & KMOD_RSHIFT) || (mod & KMOD_LSHIFT) )
	step=-14;
    cap_cam_addCrossX(1,step);
    ad->getCam2CrossX();
  }
  static void NewCrossCam1Right(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    int step = 2;
    SDLMod mod = SDL_GetModState();
    if((mod & KMOD_RSHIFT) || (mod & KMOD_LSHIFT))
      step=14;
    cap_cam_addCrossX(1,step);
    ad->getCam2CrossX();
  }

  static void NewMainEscape(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    ad->Parent->showArbeitsDialog();
  }
  static void NewMainReturn(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    ad->copyValuesToRezept();
    if(ad->getStep()<8)
      ad->incStep();
  }
  static void NewMainLeft(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    if(ad->getStep()>=0)
      ad->decStep();
  }
  static void NewMainRight(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    if(ad->getStep()<8)
      ad->incStep();
  }
  static void NewMainF8(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    ad->preparePhaseNewDirect();
  }
  static void NewMainF10(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    if(ad->getStep()==0)
      ad->incStep();
    ad->theMenuModus=NewDialog::iCrossaireMenu;
    ad->setMenuBarForNewCross();
    //ad->Label_Menu->setText(NewDialogCrossMenuText);
    //ad->Label_Menu->show(ad->Parent->theGUI->getMainSurface());
  }
  static void NewMainF12(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;
    if(ad->verifyName())//Prüfen und 
      {
	printf("error Name ist zu kurz oder zu lang\n");
	while(ad->getStep()>0)//zurück zur Nameneingabe
	  ad->decStep();
	return;
      }
    else if(ad->keinSpeicherplatz())
      {
	ad->Parent->showErrorDialog((char*)"Error, no space left");
	return;
      }
    else
      {
	if(ad->tmpRezept->writeToFile(ad->Parent->pcSaveFilePath))
	  {
	    ad->Parent->showErrorDialog((char*)"Error saving File");
	  }
	else
	  {
	    Rezept::copy(ad->tmpRezept,ad->Parent->theRezept);
	    ad->Parent->showRezept(0);
	    ad->Parent->showArbeitsDialog();
	  }
      }
  }

  static void NewMouseCrossaireListener(void * src, SDL_Event * evt)
  {
    SDL_MouseButtonEvent * mb = (SDL_MouseButtonEvent *)evt;
    NewDialog* ad = (NewDialog*)src;
    int cam = 0;
    int wert = 0;

    //printf("NewMouseCrossaireListener x:%i y:%i\n",mb->x,mb->y);
    if(mb->x > ad->sdlw/2)
      {
	cam = 1;
	wert = mb->x - (ad->sdlw/2);
      }
    else
      {
	cam = 0;
	if(ad->sdlw>=2*ad->camw)
	  wert = mb->x - (ad->sdlw/2 - ad->camw);
	else
	  wert = ad->camw - ad->sdlw/2 + mb->x;
      }
    //printf("wert:%i\n",wert);
    if(1==wert%2)
      wert++;

    cap_cam_setCrossX(cam,wert);

    if(1==cam)
      ad->getCam2CrossX();
    else
      ad->getCam1CrossX();
  }

  static void NewDialogKeyListener(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    if(ad->theMenuModus==NewDialog::iMainMenu)
      {
	if( key->type == SDL_KEYUP )
	  {
	    if(key->keysym.sym == SDLK_ESCAPE)
	      {
		NewMainEscape(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	      {
		NewMainReturn(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_LEFT || key->keysym.sym == SDLK_F5)
	      {
		NewMainLeft(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_RIGHT || key->keysym.sym == SDLK_F6)
	      {
		NewMainRight(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F8)
	      {
		NewMainF8(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F10)
	      {
		NewMainF10(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F12)
	      {
		NewMainF12(src,evt);
	      }
	  }
      }
    else //CrossaireMenu
      {
	if( key->type == SDL_KEYUP )
	  {
	    if(key->keysym.sym == SDLK_ESCAPE)
	      {
		NewCrossEscape(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	      {
		NewCrossEnter(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F1)
	      {
		NewCrossCircleMinus(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F2)
	      {
		NewCrossCirclePlus(src,evt);;
	      }
	    else if(key->keysym.sym == SDLK_F3)
	      {
		NewCrossCam0Left(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F4)
	      {
		NewCrossCam0Right(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F5)
	      {
		NewCrossCam1Left(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F6)
	      {
		NewCrossCam1Right(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F7)
	      {
		//NewCrossCam1Right(src,evt);
	      }
	    else if(key->keysym.sym == SDLK_F8)
	      {
		//NewCrossCam1Right(src,evt);
	      }
	  }
      }
  }

  NewDialog::NewDialog(int sdlw,					\
		       int sdlh,					\
		       int camw,					\
		       int camh,					\
		       int yPos,					\
		       ArbeitsDialog * parent):Screen()
  {
    short M_y;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y;
    short Spalte1_x, Spalte2_x, Spalte3_x;

    short Button_w = 332;
    short x_space = 8;

    this->Parent = parent;
    this->step = 0;
    this->theMenuModus = iMainMenu;
    this->sdlw = sdlw;
    this->camw = camw;
    this->tmpRezept = new Rezept();
    this->newDirect = new NewDirectDialog(sdlw,sdlh,camw,camh,yPos,this);
    Rezept::copy(Parent->getNullRezept(),tmpRezept);

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

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    Spalte1_x = sdlw/2 - 506;
    Spalte2_x = Spalte1_x + 1*Button_w+1*x_space;
    Spalte3_x = Spalte1_x + 2*Button_w+2*x_space;

    Label_Name = new Label("---",Spalte1_x,Zeile1_y,Button_w,MZeile_h,Parent->MenuSet);

    Label_Info = new Label("Recipe Name",Spalte3_x,Zeile1_y,Button_w,MZeile_h,Parent->DialogSet);
    snprintf(this->InfoText,64,"Recipe Name");
    Label_Info->setText(this->InfoText);

    TextField_Name = new TextField(0,LoadDialog::MaxRezeptFileLaenge,	\
				   Spalte2_x,				\
				   Zeile1_y,Button_w,			\
				   MZeile_h,				\
				   Parent->WerteSet);
    TextField_Name->setFont(Globals::getFontButtonBig());
    TextField_Name->setActive(true);

    LabelXaxisText = new Label(Parent->TextCam1Xaxis,		\
			       Spalte1_x,Zeile2_y,Button_w,MZeile_h);
    LabelZaxisText = new Label(Parent->TextCam2Xaxis,		\
			       Spalte1_x,Zeile3_y,Button_w,MZeile_h);
    LabelCrossText = new Label(Parent->TextZaxis,		\
			       Spalte1_x,Zeile4_y,Button_w,MZeile_h);

    for(int i =0;i<3;i++)
      {
	pcWerte[i][0] = '-';
	pcWerte[i][1] = '-';
	pcWerte[i][2] = '-';
	pcWerte[i][3] = '\0';
      }
    for(int i=0;i<6;i++)
      {
    	usWerte[i]=0;
      }

    LabelWerte[NewDialog::iPosX1] = new Label(pcWerte[NewDialog::iPosX1],\
					      Spalte2_x,Zeile2_y,Button_w,MZeile_h,\
					      Parent->WerteSet);
    LabelWerte[NewDialog::iPosX2] = new Label(pcWerte[NewDialog::iPosX2],\
			      Spalte2_x,Zeile3_y,Button_w,MZeile_h,\
					      Parent->WerteSet);
    LabelWerte[NewDialog::iPosZ] = new Label(pcWerte[NewDialog::iPosZ],\
			      Spalte2_x,Zeile4_y,Button_w,MZeile_h,\
					      Parent->WerteSet);

    snprintf(pcRezept[NewDialog::iPosX1],64,"%i",			\
	     tmpRezept->Rezepte[this->step].cams[0].x_pos);
    snprintf(pcRezept[NewDialog::iPosZ],64,"%i",\
	     tmpRezept->Rezepte[this->step].cams[0].z_pos);

    snprintf(pcRezept[NewDialog::iPosX2],64,"%i",\
	     tmpRezept->Rezepte[this->step].cams[1].x_pos);

    LabelRezept[NewDialog::iPosX1] = new Label(pcRezept[NewDialog::iPosX1],\
			       Spalte3_x,Zeile2_y,Button_w,MZeile_h,\
					      Parent->WerteSet);
    LabelRezept[NewDialog::iPosX2] = new Label(pcRezept[NewDialog::iPosX2],\
			       Spalte3_x,Zeile3_y,Button_w,MZeile_h,\
					      Parent->WerteSet);
    LabelRezept[NewDialog::iPosZ] = new Label(pcRezept[NewDialog::iPosZ], \
			       Spalte3_x,Zeile4_y,Button_w,MZeile_h,\
					      Parent->WerteSet);
    theMenuBarSettings.evtSource = (void*)this;

    theMenu = new MenuBar((int)Spalte1_x,(int)Zeile5_y,(int)MZeile_h,(char*)"New", \
			  &this->theMenuBarSettings,Parent);
    this->setMenuBarForNewMain();

    this->MouseCrossaire = new EvtTarget();
    this->MouseCrossaire->PosDimRect.x = sdlw/2-camw;
    this->MouseCrossaire->PosDimRect.y = 0;
    this->MouseCrossaire->PosDimRect.w = camw*2;
    this->MouseCrossaire->PosDimRect.h = camh;
    this->MouseCrossaire->setLMButtonUpEvtHandler(NewMouseCrossaireListener,this);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(NewDialogKeyListener);

    this->addEvtTarget(Label_Name);//das ist das letzte Target, welches für alle gilt
    this->addEvtTarget(this);
    theMenu->addToEvtTarget(this);
    this->addEvtTarget(Label_Info);
    this->addEvtTarget(TextField_Name);
    this->addEvtTarget(MouseCrossaire);
    //this->addEvtTarget(this->Label_MenuTitel);

    //Enter Name
    //this->TextField_Name->addEvtTarget(Label_Info);

    //RecipeSteps
    LabelXaxisText->addEvtTarget(LabelZaxisText);
    LabelXaxisText->addEvtTarget(LabelCrossText);
    LabelXaxisText->addEvtTarget(LabelWerte[NewDialog::iPosX1]);
    LabelXaxisText->addEvtTarget(LabelWerte[NewDialog::iPosX2]);
    LabelXaxisText->addEvtTarget(LabelWerte[NewDialog::iPosZ]);
    LabelXaxisText->addEvtTarget(LabelRezept[NewDialog::iPosX1]);
    LabelXaxisText->addEvtTarget(LabelRezept[NewDialog::iPosX2]);
    LabelXaxisText->addEvtTarget(LabelRezept[NewDialog::iPosZ]);
    preparePhaseEnterName();
  }

  void NewDialog::setMenuBarForNewMain()
  {

    theMenuBarSettings.Text[0]=(char *)"F8 direct";
    theMenuBarSettings.Text[1]=(char *)"F10 cross";
    theMenuBarSettings.Text[2]=(char *)"F12 save";
    theMenuBarSettings.Text[3]= 0;
    theMenuBarSettings.Text[4]=(char *)"F5 prev";
    theMenuBarSettings.Text[5]=(char *)"F6 next";
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtFnks[0]=NewMainF8;
    theMenuBarSettings.evtFnks[1]=NewMainF10;
    theMenuBarSettings.evtFnks[2]=NewMainF12;
    theMenuBarSettings.evtFnks[3]=0;
    theMenuBarSettings.evtFnks[4]=NewMainLeft;
    theMenuBarSettings.evtFnks[5]=NewMainRight;
    theMenuBarSettings.evtFnks[6]=NewMainEscape;
    theMenuBarSettings.evtFnks[7]=NewMainReturn;
    theMenu->updateSettings(&this->theMenuBarSettings);
  }

  void NewDialog::setMenuBarForNewCross()
  {

    theMenuBarSettings.Text[0]=(char *)"F1 O-";
    theMenuBarSettings.Text[1]=(char *)"F2 O+";
    theMenuBarSettings.Text[2]=(char *)"F3 <";
    theMenuBarSettings.Text[3]=(char *)"F4 >";
    theMenuBarSettings.Text[4]=(char *)"F5 <";
    theMenuBarSettings.Text[5]=(char *)"F6 >";
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtFnks[0]=NewCrossCircleMinus;
    theMenuBarSettings.evtFnks[1]=NewCrossCirclePlus;
    theMenuBarSettings.evtFnks[2]=NewCrossCam0Left;
    theMenuBarSettings.evtFnks[3]=NewCrossCam0Right;
    theMenuBarSettings.evtFnks[4]=NewCrossCam1Left;
    theMenuBarSettings.evtFnks[5]=NewCrossCam1Right;
    theMenuBarSettings.evtFnks[6]=NewCrossEscape;
    theMenuBarSettings.evtFnks[7]=NewCrossEnter;
    theMenu->updateSettings(&this->theMenuBarSettings);
  }

  void NewDialog::resetEvtTargets()
  {   
    Parent->blankMenuArea();
  }

  void NewDialog::preparePhaseEnterName()
  {
    resetEvtTargets();
    Label_Name->setText("Enter filename :");
    this->Label_Name->Next = 0;
  }

  void NewDialog::preparePhaseRecipeSteps()
  {
    resetEvtTargets();
    Label_Name->setText("Adjust recipe steps :");
    this->Label_Name->Next = this->LabelXaxisText;
  }

  void NewDialog::preparePhaseNewDirect()
  {
    int rzpStep = this->step - 1;
    if(rzpStep<0)
      {
	rzpStep = 0;
      }
    resetEvtTargets();
    Parent->theGUI->activateScreen(newDirect);
    newDirect->useNewDirectDialog(&this->tmpRezept->Rezepte[rzpStep]);
  }

  LL * NewDialog::getDialogsEvtTargets()
  {
    preparePhaseEnterName();
    return this->EvtTargets.Next;
  }

  bool NewDialog::verifyName()
  {
    if(TextField_Name->getTextLen()<=2)
      return true;
    strcpy(tmpRezept->Name,TextField_Name->getText());
    return false;
  }

  bool NewDialog::keinSpeicherplatz()
  {
    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;
    dirp = opendir(Parent->pcSaveFilePath);
    if(dirp <= 0){
      perror("keinSeicherplatz()\n");
      return false;
    }

    while ((entry = readdir(dirp)) != NULL) {
      if (entry->d_type == DT_REG) { /* If the entry is a regular file */
	file_count++;
      }
    }
    closedir(dirp);

    //printf("NewDialog::keinSpeicherplatz : file:%i\n",file_count);
    if (file_count < 0)
      return true;
    if(file_count>5000)
      return true;

    return false;
  }

  void NewDialog::setNewPositionValue(int pos, unsigned int value,char* suffix)
  {
    this->usWerte[pos] = value;
    this->LabelWerte[pos]->setText(this->Parent->int2string(this->pcWerte[pos],	\
							    32,(int)value,\
							    suffix));
    //Label::showLabel((void*)this->LabelWerte[pos],this->Parent->theGUI->getMainSurface());
    this->show(Parent->theGUI->getMainSurface());
  }

  void NewDialog::updateRezeptData()
  {
    int rzpStep = this->step - 1;

    if(this->step)
      {
	//cross aus den Rezept Daten holen
	cap_cam_setCrossX(0,\
			  tmpRezept->Rezepte[rzpStep].cams[0].x_cross);
	cap_cam_setCrossX(1,\
			  tmpRezept->Rezepte[rzpStep].cams[1].x_cross);
	//cross Werte updaten
	getCam1CrossX();
	getCam2CrossX();

	LabelRezept[NewDialog::iPosX1]->setText(		    \
		Parent->int2string(pcRezept[NewDialog::iPosX1],64, \
				   tmpRezept->Rezepte[rzpStep].cams[0].x_pos,\
				   (char*)" mm"));

	LabelRezept[NewDialog::iPosZ]->setText(			\
		 Parent->int2string(pcRezept[NewDialog::iPosZ],64, \
				    tmpRezept->Rezepte[rzpStep].cams[0].z_pos*this->Parent->getFaktorZAchse(), \
				    (char*)" mm"));

	LabelRezept[NewDialog::iPosX2]->setText(			\
		 Parent->int2string(pcRezept[NewDialog::iPosX2],64, \
				    tmpRezept->Rezepte[rzpStep].cams[1].x_pos,\
				    (char*)" mm"));
	//Durchmesser der Kreise
	this->Parent->newCircleDiameterFnk(tmpRezept->Rezepte[rzpStep].cams[0].diameter);
      }
    else
      {
	LabelRezept[NewDialog::iPosX1]->setText("");
	LabelRezept[NewDialog::iPosZ]->setText("");
	LabelRezept[NewDialog::iPosX2]->setText("");
      }
  }

  void NewDialog::copyValuesToRezept()
  {
    int rzpStep = this->step - 1;
    if(this->step)
      {
	tmpRezept->Rezepte[rzpStep].cams[0].x_pos = usWerte[NewDialog::iPosX1];
	tmpRezept->Rezepte[rzpStep].cams[0].z_pos = usWerte[NewDialog::iPosZ];
	tmpRezept->Rezepte[rzpStep].cams[1].x_pos = usWerte[NewDialog::iPosX2];

	tmpRezept->Rezepte[rzpStep].cams[0].diameter = this->Parent->getCircleDiameterFnk();
	tmpRezept->Rezepte[rzpStep].cams[1].diameter = this->Parent->getCircleDiameterFnk();

	tmpRezept->Rezepte[rzpStep].cams[0].x_cross = usWerte[NewDialog::iPosFP1];
	tmpRezept->Rezepte[rzpStep].cams[1].z_pos = usWerte[NewDialog::iPosZ2];
	tmpRezept->Rezepte[rzpStep].cams[1].x_cross = usWerte[NewDialog::iPosFP2];
	updateRezeptData();
      } 
  }

  void NewDialog::getCam1CrossX()
  {
    usWerte[NewDialog::iPosFP1] = cap_cam_getCrossX(0);
  }

  void NewDialog::getCam2CrossX()
  {
    usWerte[NewDialog::iPosFP2] = cap_cam_getCrossX(1);
  }

  void NewDialog::incStep()
  {
    static bool WechselZuPhaseStep = false;
    if(this->step==0)
      WechselZuPhaseStep = true;
    else
      WechselZuPhaseStep = false;

    if(this->step<8)
      {
	this->step++;
      }
    snprintf(this->InfoText,256, "Recipe step %i",this->step);

    if(this->step!=0&&WechselZuPhaseStep)
      {
	preparePhaseRecipeSteps();
	TextField_Name->setActive(false);
	for(int i =0;i<3;i++)
	  {
	    LabelWerte[i]->setBorder(true);
	  }
      }

    updateRezeptData();
    this->Label_Info->setText(this->InfoText);
  }
  
  void NewDialog::decStep()
  {
    if(this->step>0)
      {
	this->step--;	
      }
    if( this->step==0)
      {
	preparePhaseEnterName();
	this->TextField_Name->setActive(true);
	snprintf(this->InfoText,256,"Recipe name");
	for(int i =0;i<3;i++)
	  {
	    LabelWerte[i]->setBorder(true);
	  }
      }
    else
      {
	snprintf(this->InfoText,256,					\
		 "Recipe step %i",this->step);
      }
    updateRezeptData();
    this->Label_Info->setText(this->InfoText);
  }
  
  int NewDialog::getStep()
  {
    return this->step;
  }

  void NewDialog::newDirectReturn(PositionSet * Set)
  {
    int rzpStep = this->step - 1;
    if(Set)
      {
	if(rzpStep<0)
	  rzpStep=0;
	//copy PositionSet to Actual PositionSet in NewDialog
	memcpy(&this->tmpRezept->Rezepte[rzpStep],Set,sizeof(PositionSet));
      }
    preparePhaseRecipeSteps();
    if(getStep()==0)
      incStep();
    else
      {
	decStep();
	incStep();
      }
    Parent->theGUI->activateScreen(this);//Parent->theNewDialog);
  }

  static void Options1(void * src, SDL_Event * evt)
  {
    OptionsDialog* ad = (OptionsDialog*)src;
    ad->Parent->showFadenkreuzDialog();
  }

 static void Options5(void * src, SDL_Event * evt)
  {
    OptionsDialog* ad = (OptionsDialog*)src;
    ad->Parent->sendProtocolMsg(nPEC_ALRESET);
  }

  static void Options7(void * src, SDL_Event * evt)
  {
    OptionsDialog* ad = (OptionsDialog*)src;
    ad->Parent->showCalibrationDialog();
  }

  static void OptionsESC(void * src, SDL_Event * evt)
  {
    OptionsDialog* ad = (OptionsDialog*)src;
    ad->Parent->showArbeitsDialog();
  }

  static void OptionsDialogKeyListener(void * src, SDL_Event * evt)
  {
    //OptionsDialog* ad = (OptionsDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;


    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE || \
	   key->keysym.sym == SDLK_RETURN || \
	   key->keysym.sym == SDLK_KP_ENTER)
	  {
	    OptionsESC(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F1)
	  {
	    Options1(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F5)
	  {
	    Options5(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F7)
	  {
	    Options7(src,evt);
	  }
      }
  }

  OptionsDialog::OptionsDialog(int sdlw,		\
			       int sdlh,				\
			       int camw,				\
			       int camh,				\
			       int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y;
    short Spalte1_x, Spalte2_x, Spalte3_x;

    short Button_w = 332;
    short x_space = 8;

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

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    Spalte1_x = sdlw/2 - 506;
    Spalte2_x = Spalte1_x + 1*Button_w+1*x_space;
    Spalte3_x = Spalte1_x + 2*Button_w+2*x_space;

    this->Parent = parent;

    theMenuBarSettings.Text[0]=(char *)"F1 cross";
    theMenuBarSettings.Text[1]=0;
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=0;
    theMenuBarSettings.Text[4]=(char *)"F5 alert";
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"F7 calib";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=Options1;
    theMenuBarSettings.evtFnks[1]=0;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=0;
    theMenuBarSettings.evtFnks[4]=Options5;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=Options7;
    theMenuBarSettings.evtFnks[7]=OptionsESC;

    theMenu = new MenuBar(Spalte1_x,Zeile5_y,MZeile_h,(char*)"Options",	\
			  &this->theMenuBarSettings,parent);

    this->pTSource = this;//EvtTarget Quelle setzen
    this->EvtTargetID=(char*)"OptionsDialog";
    this->setKeyboardUpEvtHandler(OptionsDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    theMenu->addToEvtTarget(this);
  }
}
