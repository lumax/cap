/*
Bastian Ruppert
*/
//read directories
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <termios.h>

#include <string.h>

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

#include "Dialoge.h"

namespace EuMax01
{

  static void CalibrateDialogKeyListener(void * src, SDL_Event * evt)
  {
    CalibrationDialog* ad = (CalibrationDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->Parent->showArbeitsDialog();
	  }
	else if(key->keysym.sym == SDLK_RETURN)
	  {
	    ad->resetStepValue();
	  }
	else if(key->keysym.sym == SDLK_LEFT)
	  {
	    ad->decStep();
	  }
	else if(key->keysym.sym == SDLK_RIGHT)
	  {
	    ad->incStep();
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
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y;

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
    //Rezepte_w = 108;

    Label_TitleName = new Label("CALIBRATION",MLinks_x,Zeile1_y,506*2,MZeile_h);
    Label_TitleName->setFont(Globals::getFontButtonBig());

    snprintf(this->StepText,256,\
	     "Move camera 1 X-Axis in zero position :");
    Label_Step = new Label(this->StepText,			\
			   MLinks_x,Zeile2_y,506*2,MZeile_h);

    snprintf(this->ValueName,16,"Q1");
    Label_ValueName = new Label(this->ValueName,			\
				MLinks_x,Zeile3_y,			\
				506-MSpace_h,MZeile_h);  
    

    snprintf(this->Value,64,"---");
    Label_Value = new Label(Value,					\
			    MLinks_x+506+MSpace_h,			\
			    Zeile3_y,506-MSpace_h,			\
			    MZeile_h);

    snprintf(this->InfoText,256,				       \
	     "RETURN : set zero position | "			       \
	     "LEFT previous step | RIGHT next step");
    Label_Menue = new Label(this->InfoText,			 \
			    MLinks_x,Zeile4_y,			 \
			    506*2,MZeile_h);

    addEvtTarget(Label_TitleName);
    addEvtTarget(Label_Step);
    addEvtTarget(Label_ValueName);
    addEvtTarget(Label_Value);
    addEvtTarget(Label_Menue);

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
	  prt_sendmsg_uint(nPEC_RESET_Q1,0x00);
	  prt_sendmsg_uint(nPEC_GET_Q1,0x00);
	  break;
	}
      case CalibrationDialog::iQ2:
	{
	  prt_sendmsg_uint(nPEC_RESET_Q2,0x00);
	  prt_sendmsg_uint(nPEC_GET_Q2,0x00);
	  break;
	}
      case CalibrationDialog::iZ1:
	{
	  prt_sendmsg_uint(nPEC_RESET_Z1,0x00);
	  prt_sendmsg_uint(nPEC_GET_Z1,0x00);
	  break;
	}
      case CalibrationDialog::iZ2:
	{
	  prt_sendmsg_uint(nPEC_RESET_Z2,0x00);
	  prt_sendmsg_uint(nPEC_GET_Z2,0x00);
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
    Label::showLabel((void*)this->Label_Value,			\
		     this->Parent->theGUI->getMainSurface());

    switch(theStep)
      {
      case CalibrationDialog::iQ1:
	{
	  this->ActualStep = theStep;

	  snprintf(this->StepText,256,				\
		   "Move camera 1 X-Axis in zero position :");
	  this->Label_Step->setText(this->StepText);

	  snprintf(this->ValueName,16,"Q1");
	  this->Label_ValueName->setText(this->ValueName);
	  prt_sendmsg_uint(nPEC_GET_Q1,0x00);
	  break;
	}
      case CalibrationDialog::iQ2:
	{
	  this->ActualStep = theStep;

	  snprintf(this->StepText,256,				\
		   "Move camera 2 X-Axis in zero position :");
	  this->Label_Step->setText(this->StepText);


	  snprintf(this->ValueName,16,"Q2");
	  this->Label_ValueName->setText(this->ValueName);
	  prt_sendmsg_uint(nPEC_GET_Q2,0x00);
	  break;
	}
      case CalibrationDialog::iZ1:
	{
	  this->ActualStep = theStep;

	  snprintf(this->StepText,256,				\
		   "Move Z-Axis in zero position :");
	  this->Label_Step->setText(this->StepText);


	  snprintf(this->ValueName,16,"Z1");
	  this->Label_ValueName->setText(this->ValueName);
	  prt_sendmsg_uint(nPEC_GET_Z1,0x00);
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
	  prt_sendmsg_uint(nPEC_GET_Z2,0x00);
	  break;
	}
      }
    Label::showLabel((void*)this->Label_Step,			\
		     this->Parent->theGUI->getMainSurface());
    Label::showLabel((void*)this->Label_ValueName,		\
		     this->Parent->theGUI->getMainSurface());
  }
  
  void CalibrationDialog::setQ1(unsigned short dat)
  {
    setXXData(dat,CalibrationDialog::iQ1);  
  }

  void CalibrationDialog::setQ2(unsigned short dat)
  {
    setXXData(dat,CalibrationDialog::iQ2);    
  }

  void CalibrationDialog::setZ1(unsigned short dat)
  {
    setXXData(dat,CalibrationDialog::iZ1);    
  }

  void CalibrationDialog::setZ2(unsigned short dat)
  {
     setXXData(dat,CalibrationDialog::iZ2);   
  }

  void CalibrationDialog::setXXData(unsigned short dat,int MyStep)
  {
    if(MyStep==this->ActualStep)
      {
	sprintf(this->Value,"%i",dat);
	this->Label_Value->setText(this->Value);
	Label::showLabel((void*)this->Label_Value,		\
			 this->Parent->theGUI->getMainSurface());
      }
  }

  /*  ArbeitsDialog */

  static void ArbeitsDialogKeyListener(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    //char zeichen = 0;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_F1)
	  {
	    //ad->theGUI->activateScreen(ad->getLoadDialog());
	    ad->showLoadDialog(0);
	  }
	else if(key->keysym.sym == SDLK_F2)
	  {
	    //printf("F2\n");
	    //	    printf("ad->theProtocol->enableAuto(); returns: %i\n", 
	    //   ad->theProtocol->enableAuto());
	    //prt_sendmsg_uint(nPEC_RESET_Q1,0x00);
	    //prt_sendmsg_uint(nPEC_GET_Q1,0x00);
	    Rezept::copy(ad->theRezept,ad->theNewDialog->tmpRezept);
	    ad->showNewDialog((char*)"Save as");
	  }
	else if(key->keysym.sym == SDLK_F3)
	  {
	    Rezept::copy(ad->getNullRezept(),ad->theNewDialog->tmpRezept);
	    ad->showNewDialog((char*)"New");
	  }
	else if(key->keysym.sym == SDLK_F4)
	  {
	    printf("F4\n");
	  }
	else if(key->keysym.sym == SDLK_F5||key->keysym.sym == SDLK_LEFT)
	  {
	    ad->decRezeptNummer();
	    ad->showRezept(ad->getRezeptNummer());
	    //ad->setCross1Ref();
	    //ad->setCross2Ref();
	  }
	else if(key->keysym.sym == SDLK_F6||key->keysym.sym == SDLK_RIGHT)
	  {
	    ad->incRezeptNummer();
	    ad->showRezept(ad->getRezeptNummer());
	    //ad->setCross1Ref();
	    //ad->setCross2Ref();
	  }
	else if(key->keysym.sym == SDLK_F7)
	  {
	    ad->showCalibrationDialog();
	  }
	else if(key->keysym.sym == SDLK_F12)
	  {
	    exit(0);
	  }
      }
  }

  static void ArbeitsDialogNoGUIKeyListener(void * src, SDL_Event * evt)
  {
    //ArbeitsDialog* ad = (ArbeitsDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_F1)
	  {

	  }
	else if(key->keysym.sym == SDLK_F12)
	  {
	    exit(0);
	  }
      }
  }

  ArbeitsDialog::~ArbeitsDialog(){}
  ArbeitsDialog::ArbeitsDialog(GUI * pGUI,		\
			       MBProtocol *prot,	\
			       int sdlw,		\
			       int sdlh,		\
			       int camw,		\
			       int camh,			\
			       int yPos,			\
			       char * saveFilePath,\
			       bool useTheGUI)//:Screen()
  {
    short M_y;
    short MLinks_x;
    short MRechts_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    short MName_y,MInfo_y;

    unsigned short MName_w;
    unsigned short MNameNr_w;
    short MNameSpace_w;
    short MNameNr1_x,MNameNr2_x,MNameNr3_x,MNameNr4_x,MNameNr5_x,MNameNr6_x,MNameNr7_x,MNameNr8_x;

    unsigned short MInfo_w;
    short MInfoSpace_w;
    short MInfoF1_x,MInfoF2_x,MInfoF3_x,MInfoF5_x,MInfoF6_x,MInfoF7_x,MInfoF8_x,MInfoF12_x;

    RezeptNummer = 0;
    iActiveDialog = 0;
    Cam1XaxisCur = 5;
    Cam1XaxisDif = 5;
    Cam2XaxisCur = 6;
    Cam2XaxisDif = 6;

    Cam1ZaxisCur = 0;
    Cam1ZaxisDif = 0;
    Cam2ZaxisCur = 0;
    Cam2ZaxisDif = 0;

    this->useGUI = useTheGUI; 
    this->theRezept = new Rezept();
    this->pNullRezept = new Rezept();
    this->theGUI = pGUI;
    this->theProtocol = prot;
    this->pcSaveFilePath = saveFilePath;
    this->pNullRezept->Name[0]='\0';
    this->setColors();

    for(unsigned int i = 0;i<LoadDialog::MaxRezeptFileLaenge;i++)
      {
	this->theRezept->Rezepte[i].cams[0].x_pos = 0;
	this->theRezept->Rezepte[i].cams[1].x_pos = 0;
	this->theRezept->Rezepte[i].cams[0].z_pos = 0;
	this->theRezept->Rezepte[i].cams[1].z_pos = 0;
	this->theRezept->Rezepte[i].cams[0].x_cross = ArbeitsDialog::MitteCrossCam1;
	this->theRezept->Rezepte[i].cams[1].x_cross = ArbeitsDialog::MitteCrossCam2;

	this->pNullRezept->Rezepte[i].cams[0].x_pos = 0;
	this->pNullRezept->Rezepte[i].cams[1].x_pos = 0;
	this->pNullRezept->Rezepte[i].cams[0].z_pos = 0;
	this->pNullRezept->Rezepte[i].cams[1].z_pos = 0;
	this->pNullRezept->Rezepte[i].cams[0].x_cross = ArbeitsDialog::MitteCrossCam1;
	this->pNullRezept->Rezepte[i].cams[1].x_cross = ArbeitsDialog::MitteCrossCam2;
	}
    theLoadDialog = new LoadDialog(sdlw,sdlh,camw,camh,yPos,this);
    theErrorDialog = new ErrorDialog(sdlw,sdlh,camw,camh,yPos,this);
    theNewDialog = new NewDialog(sdlw,sdlh,camw,camh,yPos,this);
    theCalDialog = new CalibrationDialog(sdlw,sdlh,camw,camh,yPos,this);

    cap_cam_setCrossX(0,ArbeitsDialog::MitteCrossCam1);
    cap_cam_setCrossX(1,ArbeitsDialog::MitteCrossCam2);

    MLinks_x = sdlw/2 - 506;
    MRechts_x = sdlw/2 + 6;

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

    Area.x = 0;
    Area.y = yPos;
    Area.w = sdlw;
    Area.h = M_y;

    MName_y = yPos + 2*MSpace_h + 1*MZeile_h;
    MInfo_y = yPos + 4*MSpace_h + 4*MZeile_h+5;

    if(M_y<=84)
      {
	MName_w = 90;     //5*MZeile_h
	MNameNr_w = 54;   //3*MZeile_h
	MNameSpace_w = 61;//(1012 - (MName_w+MNameNr_w*8))/8 = 61,25
      }
    else
      {
	MName_w = 112;     //4*MZeile_h
	MNameNr_w = 56;    //2*MZeile_h
	MNameSpace_w = 56; //(1012 - (MName_w+MNameNr_w*8))/8 = 56
      }

    MNameNr1_x = MLinks_x + MName_w + 1*MNameSpace_w + 0*MNameNr_w;
    MNameNr2_x = MLinks_x + MName_w + 2*MNameSpace_w + 1*MNameNr_w;
    MNameNr3_x = MLinks_x + MName_w + 3*MNameSpace_w + 2*MNameNr_w;
    MNameNr4_x = MLinks_x + MName_w + 4*MNameSpace_w + 3*MNameNr_w;
    MNameNr5_x = MLinks_x + MName_w + 5*MNameSpace_w + 4*MNameNr_w;
    MNameNr6_x = MLinks_x + MName_w + 6*MNameSpace_w + 5*MNameNr_w;
    MNameNr7_x = MLinks_x + MName_w + 7*MNameSpace_w + 6*MNameNr_w;
    MNameNr8_x = MLinks_x + MName_w + 8*MNameSpace_w + 7*MNameNr_w;

    MInfoSpace_w = 3;
    MInfo_w = (1012-(6*MInfoSpace_w))/8;
    MInfoF1_x  = MLinks_x + 0*MInfo_w + 0*MInfoSpace_w;
    MInfoF2_x  = MLinks_x + 1*MInfo_w + 1*MInfoSpace_w;
    MInfoF3_x  = MLinks_x + 2*MInfo_w + 2*MInfoSpace_w;
    MInfoF5_x  = MLinks_x + 3*MInfo_w + 3*MInfoSpace_w;
    MInfoF6_x  = MLinks_x + 4*MInfo_w + 4*MInfoSpace_w;
    MInfoF7_x  = MLinks_x + 5*MInfo_w + 5*MInfoSpace_w;
    MInfoF8_x  = MLinks_x + 6*MInfo_w + 6*MInfoSpace_w;
    MInfoF12_x = MLinks_x + 7*MInfo_w + 7*MInfoSpace_w;

    thePosDialog = new PosDialog(theRezept->Name,sdlw,sdlh,camw,camh,yPos,this);

    LabelDialogName = new Label("Recipe",MInfoF1_x,MInfo_y,MInfo_w,MZeile_h);
    LabelDialogName->setFont(Globals::getFontButtonBig());
    LabelDialogName->setNormalColor(ArbeitsDialog::uiC_MenuNormal);
    LabelDialogName->setMarkedColor(ArbeitsDialog::uiC_MenuMarked);
    LabelDialogName->setFontColor(ArbeitsDialog::pC_MenuText);

    LabelInfo = new Label("F1: load | F2: save as | F3: new | "		\
			  "F5: prev step | F6: next step | "		\
			  "F7: calibrate | F12: exit",			\
			  MInfoF2_x,					\
			  MInfo_y,					\
			  MInfo_w*7+6*MInfoSpace_w,			\
			  MZeile_h);
    LabelInfo->setNormalColor(ArbeitsDialog::uiC_MenuNormal);
    LabelInfo->setMarkedColor(ArbeitsDialog::uiC_MenuMarked);
    LabelInfo->setFontColor(ArbeitsDialog::pC_MenuText);
    LabelInfo->setFont(Globals::getFontButtonBig());

    if(useGUI)
      {
      printf("guiMode true \n");

    this->pTSource = this; //EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(ArbeitsDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    
    this->addEvtTarget(&thePosDialog->EvtTargets);
    this->addEvtTarget(LabelDialogName);
    this->addEvtTarget(LabelInfo);
    
    this->ArbeitsDialogEvtTargets.Next = this->EvtTargets.Next;//EvtTargets fuer spaeter sichern
    this->showCalibrationDialog();
    this->showRezept(0);
      }
    else
      {
	printf("guiMode false \n");
	iActiveDialog = ArbeitsDialog::NoGUIModeIsActive;

	LabelDialogName->setText("F12: Exit");

	this->pTSource = this; //EvtTarget Quelle setzenfür den EvtListener
	this->setKeyboardUpEvtHandler(ArbeitsDialogNoGUIKeyListener);
	this->addEvtTarget(this);
	this->addEvtTarget(LabelDialogName);
      }
  }

  void ArbeitsDialog::setColors()
  {
    uiC_MenuNormal = Globals::GlobalUint32ColorDarkGray;
    uiC_MenuMarked = Globals::GlobalUint32ColorDarkGray;
    pC_MenuText = &Globals::GlobalSDL_Color2;

    uiC_DialogNormal = Globals::GlobalUint32Color2;
    uiC_DialogMarked = Globals::GlobalUint32Color2;
    pC_DialogText = &Globals::GlobalSDL_ColorBlack;

    uiC_WerteNormal = Globals::GlobalUint32Color3;
    uiC_WerteMarked = Globals::GlobalUint32Color4;
    pC_WerteText = &Globals::GlobalSDL_ColorBlack;
  }

  void ArbeitsDialog::showLoadDialog(unsigned int page)
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::LoadDialogIsActive;
    if(theLoadDialog->readSaveDirectory(pcSaveFilePath,page))
      {
	showErrorDialog((char*)"Error reading save directory");
	return;
      }

    this->EvtTargets.Next = this->theLoadDialog->EvtTargets.Next;
    Tool::blankSurface(this->theGUI->getMainSurface(),	\
		       FSG_BACKGROUND,			\
		       &this->Area);//TODO Rückgabewert

    SDL_UpdateRect(this->theGUI->getMainSurface(),	\
		   this->Area.x,			\
		   this->Area.y,			\
		   this->Area.w,			\
		   this->Area.h);
    this->show(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::showArbeitsDialog()
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::ArbeitsDialogIsActive;
    this->EvtTargets.Next = this->ArbeitsDialogEvtTargets.Next;
    Tool::blankSurface(this->theGUI->getMainSurface(),	\
		       FSG_BACKGROUND,			\
		       &this->Area);//TODO Rückgabewert
    SDL_UpdateRect(this->theGUI->getMainSurface(),	\
		   this->Area.x,			\
		   this->Area.y,			\
		   this->Area.w,			\
		   this->Area.h);
    this->show(this->theGUI->getMainSurface());
    this->showRezept(0);
    prt_sendmsg_uint(nPEC_GET_Q1,0x00);
    prt_sendmsg_uint(nPEC_GET_Q2,0x00);
    prt_sendmsg_uint(nPEC_GET_Z1,0x00);
    //prt_sendmsg_uint(nPEC_GET_Z2,0x00);
  }

  void ArbeitsDialog::showErrorDialog(char * msg)
  {
    if(!useGUI)//TODO errors im BlindMode evtl. zulassen
      return;
    this->iActiveDialog = ArbeitsDialog::ErrorDialogIsActive;
    this->theErrorDialog->setErrorMsg(msg);
    this->EvtTargets.Next = this->theErrorDialog->EvtTargets.Next;
    Tool::blankSurface(this->theGUI->getMainSurface(),	\
		       FSG_BACKGROUND,			\
		       &this->Area);//TODO Rückgabewert
    SDL_UpdateRect(this->theGUI->getMainSurface(),	\
		   this->Area.x,			\
		   this->Area.y,			\
		   this->Area.w,			\
		   this->Area.h);
    this->show(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::showNewDialog(char * MenuTitel)
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::NewDialogIsActive;
    this->EvtTargets.Next = this->theNewDialog->EvtTargets.Next;
    Tool::blankSurface(this->theGUI->getMainSurface(),	\
		       FSG_BACKGROUND,			\
		       &this->Area);//TODO Rückgabewert

    SDL_UpdateRect(this->theGUI->getMainSurface(),	\
		   this->Area.x,			\
		   this->Area.y,			\
		   this->Area.w,			\
		   this->Area.h);
    this->show(this->theGUI->getMainSurface());
    	prt_sendmsg_uint(nPEC_GET_Q1,0x00);
	prt_sendmsg_uint(nPEC_GET_Q2,0x00);
	prt_sendmsg_uint(nPEC_GET_Z1,0x00);
	prt_sendmsg_uint(nPEC_GET_Z2,0x00);
	//prt_sendmsg_uint(nPEC_GET_FP1,0x00);
	//prt_sendmsg_uint(nPEC_GET_FP2,0x00);
	theNewDialog->TextField_Name->setText(theNewDialog->tmpRezept->Name);
	Label::showLabel((void*)theNewDialog->TextField_Name,\
			 theGUI->getMainSurface());
	theNewDialog->Label_MenuTitle->setText(MenuTitel);
	Label::showLabel((void*)theNewDialog->Label_MenuTitle,\
			 theGUI->getMainSurface());
	this->theNewDialog->decStep();
  }

  void ArbeitsDialog::showCalibrationDialog()
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::CalDialogIsActive;
    this->EvtTargets.Next = this->theCalDialog->EvtTargets.Next;
    Tool::blankSurface(this->theGUI->getMainSurface(),	\
		       FSG_BACKGROUND,			\
		       &this->Area);//TODO Rückgabewert

    SDL_UpdateRect(this->theGUI->getMainSurface(),	\
		   this->Area.x,			\
		   this->Area.y,			\
		   this->Area.w,			\
		   this->Area.h);
    this->show(this->theGUI->getMainSurface());
    //this->theCalDialog->incStep();
  }

  void ArbeitsDialog::incRezeptNummer()
  {
    this->RezeptNummer++;
    if(this->RezeptNummer>=Rezept::AnzahlRezepte)
      this->RezeptNummer = Rezept::AnzahlRezepte-1;
  }

  void ArbeitsDialog::decRezeptNummer()
  {
    this->RezeptNummer--;
    if(this->RezeptNummer<0)
      this->RezeptNummer = 0;
  }

  int ArbeitsDialog::getRezeptNummer()
  {
    return this->RezeptNummer;
  }

  void ArbeitsDialog::setCam1XaxisCur(int val)
  {
    Cam1XaxisCur = val;
    Cam1XaxisDif = Cam1XaxisCur - this->theRezept->getXPosition(0,RezeptNummer);
    thePosDialog->showDifferenceCam1(val,this->theRezept->getXPosition(0,RezeptNummer));
    
    sprintf(this->thePosDialog->pcLabelCam1[PosDialog::iCurr],"%i",Cam1XaxisCur);
    thePosDialog->pLabelCam1[PosDialog::iCurr]->		\
      setText(thePosDialog->pcLabelCam1[PosDialog::iCurr]);
    Label::showLabel((void*)thePosDialog->pLabelCam1[PosDialog::iCurr],	\
		     this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::setCam2XaxisCur(int val)
  {
    Cam2XaxisCur = val;
    Cam2XaxisDif = Cam2XaxisCur - this->theRezept->getXPosition(1,RezeptNummer);
    thePosDialog->showDifferenceCam2(val,this->theRezept->getXPosition(1,RezeptNummer));
    
    sprintf(this->thePosDialog->pcLabelCam2[PosDialog::iCurr],"%i",Cam2XaxisCur);
    thePosDialog->pLabelCam2[PosDialog::iCurr]->		\
      setText(thePosDialog->pcLabelCam2[PosDialog::iCurr]);
    Label::showLabel((void*)thePosDialog->pLabelCam2[PosDialog::iCurr],	\
		     this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::setCam1ZaxisCur(int val)
  {
    Cam1ZaxisCur = val;
    Cam1ZaxisDif = Cam1ZaxisCur - this->theRezept->getZPosition(RezeptNummer);
    thePosDialog->showDifferenceZ(val,this->theRezept->getZPosition(RezeptNummer));
    
    sprintf(this->thePosDialog->pcLabelZ[PosDialog::iCurr],"%i",Cam1ZaxisCur);
    thePosDialog->pLabelZ[PosDialog::iCurr]->		\
      setText(thePosDialog->pcLabelZ[PosDialog::iCurr]);
    Label::showLabel((void*)thePosDialog->pLabelZ[PosDialog::iCurr],	\
		     this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::setCam2ZaxisCur(int val)
  {
    //nur eine Z-Achse vorhanden
    //printf("ArbeitsDialog::setCam2ZaxisCur(int val)\n");
  }

  void ArbeitsDialog::showRezept(int nummer)
  {
    if(theRezept)
      {
	RezeptNummer = nummer;

	cap_cam_setCrossX(0,theRezept->Rezepte[RezeptNummer].cams[0].x_cross);
	cap_cam_setCrossX(1,theRezept->Rezepte[RezeptNummer].cams[1].x_cross);

	thePosDialog->showRecipeName(theRezept->Name);

	sprintf(thePosDialog->pcLabelCam1[PosDialog::iStep],	\
		"%i",theRezept->getXPosition(0,nummer));
	thePosDialog->pLabelCam1[PosDialog::iStep]->		\
	  setText(thePosDialog->pcLabelCam1[PosDialog::iStep]);

	sprintf(thePosDialog->pcLabelCam2[PosDialog::iStep],	\
		"%i",theRezept->getXPosition(1,nummer));
	thePosDialog->pLabelCam2[PosDialog::iStep]->	\
	  setText(thePosDialog->pcLabelCam2[PosDialog::iStep]);

	sprintf(thePosDialog->pcLabelZ[PosDialog::iStep],	\
		"%i",theRezept->getZPosition(nummer));
	thePosDialog->pLabelZ[PosDialog::iStep]->	\
	  setText(thePosDialog->pcLabelZ[PosDialog::iStep]);

	switch(nummer)
	  {
	  case 0:
	    {
	      thePosDialog->LabelStep->setText("Step: 1");
	      break;
	    }
	  case 1:
	    {
	      thePosDialog->LabelStep->setText("Step: 2");
	      break;
	    }
	  case 2:
	    {
	      thePosDialog->LabelStep->setText("Step: 3");
	      break;
	    }
	  case 3:
	    {
	      thePosDialog->LabelStep->setText("Step: 4");
	      break;
	    }
	  case 4:
	    {
	      thePosDialog->LabelStep->setText("Step: 5");
	      break;
	    }
	  case 5:
	    {
	      thePosDialog->LabelStep->setText("Step: 6");
	      break;
	    }
	  case 6:
	    {
	      thePosDialog->LabelStep->setText("Step: 7");
	      break;
	    }
	  case 7:
	    {
	      thePosDialog->LabelStep->setText("Step: 8");
	      break;
	    }
	  default:
	    {
	      thePosDialog->LabelStep->setText("Step");
	      break;
	    }
	  }

	//Difference Berechnen
	this->setCam1XaxisCur(this->Cam1XaxisCur);
	this->setCam2XaxisCur(this->Cam2XaxisCur);
	this->setCam1ZaxisCur(this->Cam1ZaxisCur);
	this->setCam2ZaxisCur(this->Cam2ZaxisCur);
      }
  }

  char * ArbeitsDialog::int2string(char * tar,int len,int data)
  {
    snprintf(tar,len,"%i",data);
    return tar;
  }
  
  void ArbeitsDialog::setCross1Ref()
  {
    //sprintf(thePosDialog->getCrossRefBuf(),"%i",cap_cam_getCrossX(0));
    //Pos_Cam1->Label_CrossRef->setText(this->Pos_Cam1->getCrossRefBuf());
    printf("void ArbeitsDialog::setCross1Ref()\n");
  }

  void ArbeitsDialog::setCross2Ref()
  {
    //sprintf(this->Pos_Cam2->getCrossRefBuf(),"%i",cap_cam_getCrossX(1));
    //Pos_Cam2->Label_CrossRef->setText(this->Pos_Cam2->getCrossRefBuf());
    printf("void ArbeitsDialog::setCross2Ref()\n");
  }

  /*  void ArbeitsDialog::showRecipe(Rezept * rez)
  {
    theGUI->activateScreen((Screen*)this);
    }*/

  void ArbeitsDialog::Q1_evt(unsigned short dat)
  {
    if(iActiveDialog==ArbeitsDialog::ArbeitsDialogIsActive)
      setCam1XaxisCur(dat);
    else if(iActiveDialog==ArbeitsDialog::CalDialogIsActive)
      this->theCalDialog->setQ1(dat);
    else if(iActiveDialog==ArbeitsDialog::NewDialogIsActive)
      {
	this->theNewDialog->setNewPositionValue(NewDialog::iPosX1,dat);
      }
  }

  void ArbeitsDialog::Q2_evt(unsigned short dat)
  {
    if(iActiveDialog==ArbeitsDialog::ArbeitsDialogIsActive)
      setCam2XaxisCur(dat);
    else if(iActiveDialog==ArbeitsDialog::CalDialogIsActive)
      this->theCalDialog->setQ2(dat);
    else if(iActiveDialog==ArbeitsDialog::NewDialogIsActive)
      {
	this->theNewDialog->setNewPositionValue(NewDialog::iPosX2,dat);
      }
  }
  void ArbeitsDialog::Z1_evt(unsigned short dat)
  {
    if(iActiveDialog==ArbeitsDialog::ArbeitsDialogIsActive)
      setCam1ZaxisCur(dat);
    else if(iActiveDialog==ArbeitsDialog::CalDialogIsActive)
      this->theCalDialog->setZ1(dat);
    else if(iActiveDialog==ArbeitsDialog::NewDialogIsActive)
      {
	this->theNewDialog->setNewPositionValue(NewDialog::iPosZ,dat);
      }
  }
  void ArbeitsDialog::Z2_evt(unsigned short dat)
  {
    //Z2 gibt es nicht
    /*    if(iActiveDialog==ArbeitsDialog::ArbeitsDialogIsActive)
      setCam2ZaxisCur(dat);
    else if(iActiveDialog==ArbeitsDialog::CalDialogIsActive)
      this->theCalDialog->setZ2(dat);
    else if(iActiveDialog==ArbeitsDialog::NewDialogIsActive)
      {
	this->theNewDialog->setNewPositionValue(NewDialog::iPosZ2,dat);
	}*/
  }
  void ArbeitsDialog::convertCamPos(int cam,unsigned short dat)
  {
    int step,wert,camera;
    step =  dat / 2; // (0x3ff / 512) = 1.99805
    if(cam) //Anzeigebereich von 0-512
      {
	camera = 1;
	wert = step;
	
      }
    else   //Anzeigebereich von 288-800
      {
	camera = 0;
	wert = step+288;
      }
    if(wert%2)  //nur duch 2 teilbar zulassen
      {
	wert--;
	//printf("camera : %i , Wert: %i\n",camera,wert);
      }
    cap_cam_setCrossX(cam,wert);
  }
  void ArbeitsDialog::FP1_evt(unsigned short dat)
  {
    convertCamPos(0,dat);
    theNewDialog->getCam1CrossX();//hier werden die cross Daten zum abspeichern geholt
    //printf("ArbeitsDialog getFP1:%i\n",dat);
  }
  void ArbeitsDialog::FP2_evt(unsigned short dat)
  {
    convertCamPos(2,dat);
    theNewDialog->getCam2CrossX();//hier werden die cross Daten zum abspeichern geholt
    //printf("ArbeitsDialog getFB2:%i\n",dat);
  }
  void ArbeitsDialog::SWVersion_evt(unsigned short dat)
  {
    printf("ArbeitsDialog SWVersion:%i\n",dat);
  }
  void ArbeitsDialog::HWVersion_evt(unsigned short dat)
  {
    printf("ArbeitsDialog HWVersion:%i\n",dat);
  }

  Rezept * ArbeitsDialog::getNullRezept()
  {
    return this->pNullRezept;
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

    LabelRecipeName = new Label("",B1x,Y1,Bw,MZeile_h);
    LabelRecipeName->setNormalColor(Parent->uiC_DialogNormal);
    LabelRecipeName->setMarkedColor(Parent->uiC_DialogMarked);
    LabelRecipeName->setFontColor(Parent->pC_DialogText);
    LabelRecipeName->setFont(Globals::getFontButtonBig());

    LabelStep = new Label("Step 1",B4x,Y1,Bw,MZeile_h);
    LabelStep->setNormalColor(Parent->uiC_DialogNormal);
    LabelStep->setMarkedColor(Parent->uiC_DialogMarked);
    LabelStep->setFontColor(Parent->pC_DialogText);
    LabelStep->setFont(Globals::getFontButtonBig());

    //difference
    LabelActual = new Label("current position",B2x,Y1,Bw,MZeile_h);
    LabelActual->setNormalColor(Parent->uiC_DialogNormal);
    LabelActual->setMarkedColor(Parent->uiC_DialogMarked);
    LabelActual->setFontColor(Parent->pC_DialogText);
    LabelActual->setFont(Globals::getFontButtonBig());

    pLabelCam1[PosDialog::iLabelName] = new Label("Cam1 X-Axis",B1x,Y2,Bw,MZeile_h);
    pLabelCam2[PosDialog::iLabelName] = new Label("Cam2 X-Axis",B1x,Y3,Bw,MZeile_h);
    pLabelZ[PosDialog::iLabelName] = new Label("Z - Axis",B1x,Y4,Bw,MZeile_h);

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
	    pLabelCam1[i]->setNormalColor(Parent->uiC_WerteNormal);
	    pLabelCam1[i]->setMarkedColor(Parent->uiC_WerteMarked);
	    pLabelCam1[i]->setFontColor(Parent->pC_WerteText);
	    pLabelCam1[i]->setFont(Globals::getFontButtonBig());

	    pLabelCam2[i]->setNormalColor(Parent->uiC_WerteNormal);
	    pLabelCam2[i]->setMarkedColor(Parent->uiC_WerteMarked);
	    pLabelCam2[i]->setFontColor(Parent->pC_WerteText);
	    pLabelCam2[i]->setFont(Globals::getFontButtonBig());

	    pLabelZ[i]->setNormalColor(Parent->uiC_WerteNormal);
	    pLabelZ[i]->setMarkedColor(Parent->uiC_WerteMarked);
	    pLabelZ[i]->setFontColor(Parent->pC_WerteText);
	    pLabelZ[i]->setFont(Globals::getFontButtonBig());
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
    Label::showLabel((void*)pLabelCam1[PosDialog::iDiff],	\
		     Parent->theGUI->getMainSurface());
  }
  
  void PosDialog::showDifferenceCam2(int aktuell,int rezept)
  {
    unsigned int e =0;
    e=printDifference(pcLabelCam2[PosDialog::iDiff],aktuell,rezept);
    pLabelCam2[PosDialog::iDiff]->setMarkedColor(DiffFarben[e]);
    pLabelCam2[PosDialog::iDiff]->setText(pcLabelCam2[PosDialog::iDiff]);
    Label::showLabel((void*)pLabelCam2[PosDialog::iDiff],	\
		     Parent->theGUI->getMainSurface());
  }
  
  void PosDialog::showDifferenceZ(int aktuell,int rezept)
  {
    unsigned int e = 0;
    e=printDifference(pcLabelZ[PosDialog::iDiff],aktuell,rezept);
    pLabelZ[PosDialog::iDiff]->setMarkedColor(DiffFarben[e]);
    pLabelZ[PosDialog::iDiff]->setText(pcLabelZ[PosDialog::iDiff]);
    Label::showLabel((void*)pLabelZ[PosDialog::iDiff],	\
		     Parent->theGUI->getMainSurface());
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
    snprintf(pcLabelRecipeName,64,"Name: %s",text);
    LabelRecipeName->setText(pcLabelRecipeName);
    Label::showLabel((void*)LabelRecipeName,		\
		     Parent->theGUI->getMainSurface());
  }

  static void ErrorDialogKeyListener(void * src, SDL_Event * evt)
  {
    ErrorDialog* ad = (ErrorDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->Parent->showArbeitsDialog();
	  }
	else if(key->keysym.sym == SDLK_RETURN)
	  {
	    ad->Parent->showArbeitsDialog();
	  }
      }
  }

  ErrorDialog::ErrorDialog(int sdlw,		\
			   int sdlh,		\
			   int camw,		\
			   int camh,					\
			   int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //short MLoadName_y;

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

    //MLoadName_y  = yPos + 1*MSpace_h + 0*MZeile_h;

    Label_Error = new Label("Error",					\
			    MLinks_x,					\
			    yPos + 1*MSpace_h + 0*MZeile_h,		\
			    506*2,MZeile_h);
    
    Label_Info = new Label("----",					\
			   MLinks_x,					\
			   yPos + 2*MSpace_h + 1*MZeile_h,		\
			   506*2,MZeile_h);
    
    Label_OK = new Label("OK  (Enter)",					\
			 MLinks_x,					\
			 yPos + 4*MSpace_h + 3*MZeile_h,		\
			 506,MZeile_h);
    
    this->pTSource = this;//EvtTarget Quelle setzen
    this->setKeyboardUpEvtHandler(ErrorDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    this->addEvtTarget(Label_Error);
    this->addEvtTarget(Label_Info);
    this->addEvtTarget(Label_OK);
  }

  void ErrorDialog::setErrorMsg(char * Message)
  {
    this->Label_Info->setText(Message);
  }

  static void LoadDialogKeyListener(void * src, SDL_Event * evt)
  {
    LoadDialog* ad = (LoadDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->Parent->showArbeitsDialog();
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
	else if(key->keysym.sym == SDLK_RETURN)
	  {
	    ad->naviReturn();
	  }
	else if(key->keysym.sym == SDLK_PAGEDOWN)
	  {
	    ad->naviPagedown();
	  }
	else if(key->keysym.sym == SDLK_PAGEUP)
	  {
	    ad->naviPageup();
	  }
      }
  }

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

    Label_LadenName = new Label("Load",MLinks_x,MLoadName_y,150,MZeile_h);
    Label_LadenName->setNormalColor(Parent->uiC_MenuNormal);
    Label_LadenName->setMarkedColor(Parent->uiC_MenuMarked);
    Label_LadenName->setFontColor(Parent->pC_MenuText);
    Label_LadenName->setFont(Globals::getFontButtonBig());

    //Label_MenuText = new Label("F1 F2 usw",MLinks_x+158,MLoadName_y,1012-158,MZeile_h);

    unsigned int ii = 0;
    char tmpc[16] = { 'x','X','x','x','x','x','x','x'};
    tmpc[15]='\0';
    for(unsigned int i=0;i<LoadDialog::RezepteLen;i++)
      {
	if(ii>=LoadDialog::RezepteProZeile)
	  {
	    ii=0;
	    Rezepte_y += 1*MSpace_h + 1*MZeile_h;
	  }
	sprintf(DateiNamen[i],"%s",tmpc);                   //Text Buffer füllen
	pLabel_Rezepte[i] = new Label(DateiNamen[i],			\
				      MLinks_x + ii*x_space + ii*Rezepte_w, \
				      Rezepte_y,			\
				      Rezepte_w,			\
				      MZeile_h);
	pLabel_Rezepte[i]->setNormalColor(Parent->uiC_WerteNormal);
	pLabel_Rezepte[i]->setMarkedColor(Parent->uiC_WerteMarked);
	pLabel_Rezepte[i]->setFontColor(Parent->pC_WerteText);
	pLabel_Rezepte[i]->setFont(Globals::getFontButton10());
	ii++;
	this->addEvtTarget(pLabel_Rezepte[i]);
      }
    //setActiveRecipe(10);

    //this->addEvtTarget(Label_LadenName);
    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(LoadDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!

    //MLabels_y = yPos + 2*MSpace_h + 1*MZeile_h;
  }

  /* \brief Makiert das aktive Label und desmaskiert den Vorherigen.
   */ 
  void LoadDialog::setActiveRecipe(unsigned int nr)
  {
    pLabel_Rezepte[ActiveRecipe]->setBorder(false);
    
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
    this->show(this->Parent->theGUI->getMainSurface());
  }

  void LoadDialog::addToActiveRecipe(int summand)
  {
    int nr = this->ActiveRecipe;
    nr += summand;
    this->setActiveRecipe(nr);
  }

  static int dirFilter(const struct dirent * dir)
  {
    if(strlen(dir->d_name)>LoadDialog::MaxRezeptFileLaenge)
      {
	return 0;
      }
    if(strlen(dir->d_name)==1)
      {
	return 0;
      }
    if(strlen(dir->d_name)==2)
      {
	return 0;
      }
    return 1;
  }

  int LoadDialog::readSaveDirectory(char * dirName,unsigned int page)
  {
    struct dirent **namelist;
    unsigned int n;
    unsigned int fileToShow;

    this->EvtTargets.Next = 0;     //alle FileLabels deaktivieren
    this->Next = 0;                //alles nach dem Keylistener = 0
    this->addEvtTarget(this);      //den Screen Key Listener bei sich selber anmelden!
    this->Label_LadenName->Next = 0; //Laden Label anzeigen
    this->addEvtTarget(Label_LadenName);
    //this->Label_MenuText->Next = 0; //Laden MenuText anzeigen
    //this->addEvtTarget(Label_MenuText);
    this->ActiveSavePage = page;

    n = scandir(dirName, &namelist, dirFilter, alphasort);
    if (n < 0)
      return -1;

    unsigned int ii = n;
    this->MaxSavePages = 0;
    while(ii>=LoadDialog::RezepteLen)
      {
	this->MaxSavePages++;
	ii-=LoadDialog::RezepteLen;
      }

    fileToShow = page * LoadDialog::RezepteLen;
    if(fileToShow<=n)
      {
	//int tmp = 0;
	unsigned int i = 0;
	unsigned int i2 = 0;
	for(i=fileToShow,i2=0;i<n&&i2<LoadDialog::RezepteLen;i++,i2++)
	  {
	    strncpy(DateiNamen[/*tmp*/i2],namelist[i]->d_name,LoadDialog::MaxRezeptFileLaenge);
	    pLabel_Rezepte[i2]->setText(DateiNamen[i2]); //neuen Text setzen
	    pLabel_Rezepte[i2]->Next = 0;  // alles hinter diesem Label = 0
	    this->addEvtTarget(pLabel_Rezepte[i2]);
	    //tmp++;
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

  void LoadDialog::naviPageup()
  {
    if(this->ActiveSavePage>=1)
      this->Parent->showLoadDialog(this->ActiveSavePage-1);
  }

  void LoadDialog::naviPagedown()
  {
    if(this->ActiveSavePage<this->MaxSavePages)
      this->Parent->showLoadDialog(this->ActiveSavePage+1);
  }
  
  static char * NewDialogMainMenuText = (char*)"RETURN : take over | ESC : abort | " \
    "F10 save | F12 Cross Menu";
  //LEFT prev step | RIGHT next step | 
  static char * NewDialogCrossMenuText = (char*)"CAM1 F1: << "\
    "| F2: < | F3: > | F4: >> || "	\
    "CAM2 F5: << | F6: < | F7: > | F8: >> | ";
  
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
		ad->Parent->showArbeitsDialog();
	      }
	    else if(key->keysym.sym == SDLK_RETURN)
	      {
		ad->copyValuesToRezept();
		if(ad->getStep()<8)
		  ad->incStep();
	      }
	    else if(key->keysym.sym == SDLK_LEFT)
	      {
		if(ad->getStep()>=0)
		  ad->decStep();
	      }
	    else if(key->keysym.sym == SDLK_RIGHT)
	      {
		if(ad->getStep()<8)
		  ad->incStep();
	      }
	    else if(key->keysym.sym == SDLK_F10)
	      {
		if(ad->verifyName())//Prüfen und 
		  {
		    printf("error Name ist zu kurz oder zu lang\n");
		    while(ad->getStep()>0)//zurück zur Nameneingabe
		      ad->decStep();
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
	    else if(key->keysym.sym == SDLK_F12)
	      {
		ad->theMenuModus=NewDialog::iCrossaireMenu;
		ad->Label_Menu->setText(NewDialogCrossMenuText);
		Label::showLabel((void*)ad->Label_Menu,ad->Parent->theGUI->getMainSurface());
	      }
	  }
      }
    else //CrossaireMenu
      {
	if( key->type == SDL_KEYUP )
	  {
	    if(key->keysym.sym == SDLK_ESCAPE||key->keysym.sym == SDLK_RETURN)
	      {
		ad->theMenuModus=NewDialog::iMainMenu;
		ad->Label_Menu->setText(NewDialogMainMenuText);
		Label::showLabel((void*)ad->Label_Menu,ad->Parent->theGUI->getMainSurface());
	      }
	    else if(key->keysym.sym == SDLK_F1)
	      {
		cap_cam_addCrossX(0,-10);
		ad->getCam1CrossX();
	      }
	    else if(key->keysym.sym == SDLK_F2)
	      {
		cap_cam_addCrossX(0,-2);
		ad->getCam1CrossX();
	      }
	    else if(key->keysym.sym == SDLK_F3)
	      {
		cap_cam_addCrossX(0,2);
		ad->getCam1CrossX();
	      }
	    else if(key->keysym.sym == SDLK_F4)
	      {
		cap_cam_addCrossX(0,10);
		ad->getCam1CrossX();
	      }
	    else if(key->keysym.sym == SDLK_F5)
	      {
		cap_cam_addCrossX(1,-10);
		ad->getCam2CrossX();
	      }
	    else if(key->keysym.sym == SDLK_F6)
	      {
		cap_cam_addCrossX(1,-2);
		ad->getCam2CrossX();
	      }
	    else if(key->keysym.sym == SDLK_F7)
	      {
		cap_cam_addCrossX(1,2);
		ad->getCam2CrossX();
	      }
	    else if(key->keysym.sym == SDLK_F8)
	      {
		cap_cam_addCrossX(1,10);
		ad->getCam2CrossX();
	      }
	  }
      }
  }

  NewDialog::NewDialog(int sdlw,		\
			 int sdlh,		\
			 int camw,		\
			 int camh,		\
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

    this->Parent = parent;
    this->step = 0;
    this->theMenuModus = iMainMenu;
    this->tmpRezept = new Rezept();
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

    //Label_NewName = new Label("NEW RECIPE",MLinks_x,Zeile1_y,506*2,MZeile_h);
    Label_Name = new Label("RECIPE     Name :",Spalte1_x,Zeile1_y,Button_w,MZeile_h);
    Label_Name->setNormalColor(Parent->uiC_DialogNormal);
    Label_Name->setMarkedColor(Parent->uiC_DialogMarked);
    Label_Name->setFontColor(Parent->pC_DialogText);
    Label_Name->setFont(Globals::getFontButtonBig());

    Label_Info = new Label("Recipe Name",Spalte3_x,Zeile1_y,Button_w,MZeile_h);
    snprintf(this->InfoText,64,"Recipe Name");
    Label_Info->setText(this->InfoText);
    Label_Info->setNormalColor(Parent->uiC_DialogNormal);
    Label_Info->setMarkedColor(Parent->uiC_DialogMarked);
    Label_Info->setFontColor(Parent->pC_DialogText);
    Label_Info->setFont(Globals::getFontButtonBig());

    Label_MenuTitle = new Label("New",Spalte1_x,Zeile5_y,150,MZeile_h);
    Label_MenuTitle->setFont(Globals::getFontButtonBig());
    Label_MenuTitle->setMarkedColor(Parent->uiC_MenuMarked);
    Label_MenuTitle->setFontColor(Parent->pC_MenuText);
    Label_MenuTitle->setFont(Globals::getFontButtonBig());

    Label_Menu = new Label(NewDialogMainMenuText,			\
			   Spalte1_x+158,Zeile5_y,1012-158,MZeile_h);
    Label_Menu->setNormalColor(Parent->uiC_MenuNormal);
    Label_Menu->setMarkedColor(Parent->uiC_MenuMarked);
    Label_Menu->setFontColor(Parent->pC_MenuText);
    Label_Menu->setFont(Globals::getFontButtonBig());

    TextField_Name = new TextField(0,LoadDialog::MaxRezeptFileLaenge,	\
				   Spalte2_x,				\
				   Zeile1_y,Button_w,			\
				   MZeile_h);
    TextField_Name->setNormalColor(Parent->uiC_WerteNormal);
    TextField_Name->setMarkedColor(Parent->uiC_WerteMarked);
    TextField_Name->setFontColor(Parent->pC_WerteText);
    TextField_Name->setFont(Globals::getFontButtonBig());
    TextField_Name->setActive(true);

    LabelXaxisText = new Label("Cam1 X-Axis",Spalte1_x,Zeile2_y,Button_w,MZeile_h);
    LabelZaxisText = new Label("Cam2 X-Axis",Spalte1_x,Zeile3_y,Button_w,MZeile_h);
    LabelCrossText = new Label("Z - Axis",Spalte1_x,Zeile4_y,Button_w,MZeile_h);

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
			      Spalte2_x,Zeile2_y,Button_w,MZeile_h);
    LabelWerte[NewDialog::iPosX2] = new Label(pcWerte[NewDialog::iPosX2],\
			      Spalte2_x,Zeile3_y,Button_w,MZeile_h);
    LabelWerte[NewDialog::iPosZ] = new Label(pcWerte[NewDialog::iPosZ],\
			      Spalte2_x,Zeile4_y,Button_w,MZeile_h);

    snprintf(pcRezept[NewDialog::iPosX1],64,"%i",			\
	     tmpRezept->Rezepte[this->step].cams[0].x_pos);
    snprintf(pcRezept[NewDialog::iPosZ],64,"%i",\
	     tmpRezept->Rezepte[this->step].cams[0].z_pos);

    snprintf(pcRezept[NewDialog::iPosX2],64,"%i",\
	     tmpRezept->Rezepte[this->step].cams[1].x_pos);

    LabelRezept[NewDialog::iPosX1] = new Label(pcRezept[NewDialog::iPosX1],\
			       Spalte3_x,Zeile2_y,Button_w,MZeile_h);
    LabelRezept[NewDialog::iPosX2] = new Label(pcRezept[NewDialog::iPosX2],\
			       Spalte3_x,Zeile3_y,Button_w,MZeile_h);
    LabelRezept[NewDialog::iPosZ] = new Label(pcRezept[NewDialog::iPosZ], \
			       Spalte3_x,Zeile4_y,Button_w,MZeile_h);

    for(int i =0;i<3;i++)
      {
	LabelWerte[i]->setNormalColor(Parent->uiC_WerteNormal);
	LabelWerte[i]->setMarkedColor(Parent->uiC_WerteMarked);
	LabelWerte[i]->setFontColor(Parent->pC_WerteText);
	LabelWerte[i]->setFont(Globals::getFontButtonBig());

	LabelRezept[i]->setNormalColor(Parent->uiC_WerteNormal);
	LabelRezept[i]->setMarkedColor(Parent->uiC_WerteMarked);
	LabelRezept[i]->setFontColor(Parent->pC_WerteText);
	LabelRezept[i]->setFont(Globals::getFontButtonBig());	
      }

    addEvtTarget(Label_Name);
    addEvtTarget(Label_MenuTitle);
    addEvtTarget(TextField_Name);
    addEvtTarget(Label_Info);
    addEvtTarget(Label_Menu);
    addEvtTarget(LabelXaxisText);
    addEvtTarget(LabelZaxisText);
    addEvtTarget(LabelCrossText);
    addEvtTarget(LabelWerte[NewDialog::iPosX1]);
    addEvtTarget(LabelWerte[NewDialog::iPosX2]);
    addEvtTarget(LabelWerte[NewDialog::iPosZ]);
    addEvtTarget(LabelRezept[NewDialog::iPosX1]);
    addEvtTarget(LabelRezept[NewDialog::iPosX2]);
    addEvtTarget(LabelRezept[NewDialog::iPosZ]);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(NewDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
  }

  bool NewDialog::verifyName()
  {
    if(TextField_Name->getTextLen()<=2)
      return true;
    strcpy(tmpRezept->Name,TextField_Name->getText());
    return false;
  }

  void NewDialog::setNewPositionValue(int pos, unsigned short value)
  {
    this->usWerte[pos] = value;
    this->LabelWerte[pos]->setText(this->Parent->int2string(this->pcWerte[pos],	\
							    32,(int)value));
    Label::showLabel((void*)this->LabelWerte[pos],this->Parent->theGUI->getMainSurface());
  }

  void NewDialog::updateRezeptData()
  {
    int rzpStep = this->step - 1;
//TextField_Name->setText(tmpRezept->Name);
//Label::showLabel((void*)this->TextField_Name,this->Parent->theGUI->getMainSurface());

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

	LabelRezept[NewDialog::iPosX1]->setText(			\
		 Parent->int2string(pcRezept[NewDialog::iPosX1],64, \
				    tmpRezept->Rezepte[rzpStep].cams[0].x_pos));
	Label::showLabel((void*)LabelRezept[NewDialog::iPosX1],\
			 Parent->theGUI->getMainSurface());

	LabelRezept[NewDialog::iPosZ]->setText(			\
		 Parent->int2string(pcRezept[NewDialog::iPosZ],64, \
				    tmpRezept->Rezepte[rzpStep].cams[0].z_pos));
	Label::showLabel((void*)LabelRezept[NewDialog::iPosZ],\
			 Parent->theGUI->getMainSurface());

	LabelRezept[NewDialog::iPosX2]->setText(			\
		 Parent->int2string(pcRezept[NewDialog::iPosX2],64, \
				    tmpRezept->Rezepte[rzpStep].cams[1].x_pos));
	Label::showLabel((void*)LabelRezept[NewDialog::iPosX2],\
			 Parent->theGUI->getMainSurface());
      }
    else
      {
	LabelRezept[NewDialog::iPosX1]->setText("");
	Label::showLabel((void*)LabelRezept[NewDialog::iPosX1],	\
			 Parent->theGUI->getMainSurface());
	LabelRezept[NewDialog::iPosZ]->setText("");
	Label::showLabel((void*)LabelRezept[NewDialog::iPosZ],	\
			 Parent->theGUI->getMainSurface());
	LabelRezept[NewDialog::iPosX2]->setText("");
	Label::showLabel((void*)LabelRezept[NewDialog::iPosX2],	\
			 Parent->theGUI->getMainSurface());
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

	tmpRezept->Rezepte[rzpStep].cams[0].x_cross = usWerte[NewDialog::iPosFP1];
	tmpRezept->Rezepte[rzpStep].cams[1].z_pos = usWerte[NewDialog::iPosZ2];
	tmpRezept->Rezepte[rzpStep].cams[1].x_cross = usWerte[NewDialog::iPosFP2];
	updateRezeptData();
      } 
  }

  void NewDialog::getCam1CrossX()
  {
    usWerte[NewDialog::iPosFP1] = cap_cam_getCrossX(0);
    /*
    LabelWerte[2]->setText(Parent->int2string(pcWerte[NewDialog::iPosFP1], \
					      64,cap_cam_getCrossX(0)));
    Label::showLabel((void*)LabelWerte[2],Parent->theGUI->getMainSurface());
    */
  }

  void NewDialog::getCam2CrossX()
  {
    usWerte[NewDialog::iPosFP2] = cap_cam_getCrossX(1);
    /*
    LabelWerte[5]->setText(Parent->int2string(pcWerte[NewDialog::iPosFP2], \
					      64,cap_cam_getCrossX(1)));
    Label::showLabel((void*)LabelWerte[5],Parent->theGUI->getMainSurface());
    */
  }

  void NewDialog::incStep()
  {
    if(this->step<8)
      {
	this->step++;
      }
    snprintf(this->InfoText,256, "Recipe step %i",this->step);

    if(this->step!=0)
      {
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
	this->TextField_Name->setActive(true);
	snprintf(this->InfoText,256,"Recipe Name");
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
}
