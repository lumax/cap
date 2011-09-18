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
#include "ArbeitsDialog.h"

namespace EuMax01
{

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
	    ad->showNewDialog((char*)"Edit");
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

    this->TextCam1Xaxis =(char*)"Cam1 X-Axis";
    this->TextCam2Xaxis = (char*)"Cam2 X-Axis";
    this->TextZaxis = (char*)"Z-Axis";

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

    LabelInfo = new Label("F1: load | F2: edit | F3: new | "		\
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

    uiC_WerteNormal = Globals::GlobalUint32Color3;//ein rot
    uiC_WerteMarked = Globals::GlobalUint32Color4;//orange
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
    this->blankMenuArea();
    this->show(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::showArbeitsDialog()
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::ArbeitsDialogIsActive;
    this->EvtTargets.Next = this->ArbeitsDialogEvtTargets.Next;
    this->blankMenuArea();
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
    this->blankMenuArea();
    this->show(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::showNewDialog(char * MenuTitel)
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::NewDialogIsActive;
    this->EvtTargets.Next = this->theNewDialog->getDialogsEvtTargets();
    this->blankMenuArea();
    this->show(this->theGUI->getMainSurface());
    	prt_sendmsg_uint(nPEC_GET_Q1,0x00);
	prt_sendmsg_uint(nPEC_GET_Q2,0x00);
	prt_sendmsg_uint(nPEC_GET_Z1,0x00);
	prt_sendmsg_uint(nPEC_GET_Z2,0x00);
	//prt_sendmsg_uint(nPEC_GET_FP1,0x00);
	//prt_sendmsg_uint(nPEC_GET_FP2,0x00);
	theNewDialog->TextField_Name->setText(theNewDialog->tmpRezept->Name);
	/*	Label::showLabel((void*)theNewDialog->TextField_Name,	\
		theGUI->getMainSurface());*/
	theNewDialog->Label_MenuTitle->setText(MenuTitel);
	/*	Label::showLabel((void*)theNewDialog->Label_MenuTitle,	\
		theGUI->getMainSurface());*/
	while(this->theNewDialog->getStep()>0)
	  {
	    //step zurückspulen
	    this->theNewDialog->decStep();
	  }
  }

  void ArbeitsDialog::showCalibrationDialog()
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::CalDialogIsActive;
    this->sendProtocolMsg(nPEC_GET_Q1);
    this->EvtTargets.Next = this->theCalDialog->EvtTargets.Next;
    this->blankMenuArea();
    this->show(this->theGUI->getMainSurface());
    //this->theCalDialog->incStep();
  }

  void ArbeitsDialog::blankMenuArea()
  {
    Tool::blankSurface(this->theGUI->getMainSurface(),	\
		       FSG_BACKGROUND,			\
		       &this->Area);//TODO Rückgabewert

    SDL_UpdateRect(this->theGUI->getMainSurface(),	\
		   this->Area.x,			\
		   this->Area.y,			\
		   this->Area.w,			\
		   this->Area.h);
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

  void ArbeitsDialog::sendProtocolMsg(char cmd)
  {
    if(theProtocol)
      if(theProtocol->isInitialised())
	prt_sendmsg_uint(cmd,0x00);
  }

}//end Namespace
