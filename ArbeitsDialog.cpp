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

#include "MenuBar.h"
#include "Dialoge.h"
#include "ErrorDialog.h"
#include "InfoDialog.h"
#include "FadenkreuzDialog.h"
#include "AbfrageDialog.h"
#include "BackupDialog.h"

#include "ArbeitsDialog.h"

#include <unistd.h>


namespace EuMax01
{

  /*  ArbeitsDialog */

  static void F1_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    ad->clearFilter();
    ad->showLoadDialog(0,true);
  }

  static void F2_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    Rezept::copy(ad->theRezept,ad->theNewDialog->tmpRezept);
    ad->showNewDialog((char*)"Edit");
  }

  static void F3_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    Rezept::copy(ad->getNullRezept(),ad->theNewDialog->tmpRezept);
    ad->showNewDialog((char*)"New");
  }

  static void F4_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    ad->clearFilter();
    ad->showLoadDialog(0,false);
  }

  static void F5_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    ad->decRezeptNummer();
    ad->showRezept(ad->getRezeptNummer());
  }

  static void F6_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    ad->incRezeptNummer();
    ad->showRezept(ad->getRezeptNummer());
  }

  static void F7_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    //ad->showCalibrationDialog();
    ad->showOptionsDialog();
  }

  static void F10_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    ad->showInfoDialog();
  }

  static void F11_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    ad->theProtocol->closeProtocol();
    exit(11);
  }

  static void F12_ListenerArbeitsDialog(void * src, SDL_Event * evt)
  {
    ArbeitsDialog* ad = (ArbeitsDialog*)src;
    ad->showConfirmDialog(ad->getSplashScreen()->getExitMessage());
  }

  static void ArbeitsDialogKeyListener(void * src, SDL_Event * evt)
  {
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    SDLMod mod = key->keysym.mod;
    //char zeichen = 0;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_F1)
	  {
	    F1_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F2)
	  {
	    F2_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F3)
	  {
	    F3_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F4)
	  {
	    F4_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F5||key->keysym.sym == SDLK_LEFT)
	  {
	    F5_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F6||key->keysym.sym == SDLK_RIGHT)
	  {
	    F6_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F7)
	  {
	    F7_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F10)
	  {
	    F10_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F11)
	  {
	    F11_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F12)
	  {
	    //exit(12);
	    F12_ListenerArbeitsDialog(src,evt);
	  }
	else if(key->keysym.sym == SDLK_q)
	  {
	    if((mod & KMOD_CTRL) &&		\
	       (mod & KMOD_SHIFT)&&		\
	       (mod & KMOD_ALT))
	      {
		printf("Service\n");
	      }
	  }
      }
  }

  ArbeitsDialog::~ArbeitsDialog(){}
  ArbeitsDialog::ArbeitsDialog(GUI * pGUI,				\
			       MBProtocol *prot,			\
			       int sdlw,				\
			       int sdlh,				\
			       int camw,				\
			       int camh,				\
			       int yPos,				\
			       char * saveFilePath,			\
			       bool useTheGUI,				\
			       void (*newDiameterFnk)(int),		\
			       int (*getDiameterFnk)(void),
			       void (*setFKBreite)(int val),	\
			       void (*saveFKBreite)(void),\
			       int (*getFKBreite)(void))//:Screen()
  {
    short M_y;
    short MLinks_x;
    //short MRechts_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //short MName_y;
    short MInfo_y;

    //unsigned short MName_w;
    //unsigned short MNameNr_w;
    //short MNameSpace_w;
    //short MNameNr1_x,MNameNr2_x,MNameNr3_x,MNameNr4_x,MNameNr5_x,MNameNr6_x,MNameNr7_x,MNameNr8_x;

    unsigned short MInfo_w;
    short MInfoSpace_w;
    short MInfoF1_x;
    //MInfoF2_x,MInfoF3_x,MInfoF5_x,MInfoF6_x,MInfoF7_x,MInfoF8_x,MInfoF12_x;

    RezeptNummer = 0;
    iActiveDialog = 0;
    Cam1XaxisCur = 5;
    Cam1XaxisDif = 5;
    Cam2XaxisCur = 6;
    Cam2XaxisDif = 6;

    Cam1ZaxisCur = 1;
    Cam1ZaxisDif = 0;
    Cam2ZaxisCur = 0;
    Cam2ZaxisDif = 0;

    this->newCircleDiameterFnk = newDiameterFnk;
    this->getCircleDiameterFnk = getDiameterFnk;

    this->setCrossairWidth = setFKBreite;
    this->saveCrossairWidth = saveFKBreite;
    this->getCrossairWidth = getFKBreite;

    this->useGUI = useTheGUI; 
    this->theRezept = new Rezept();
    this->pNullRezept = new Rezept();
    this->theGUI = pGUI;
    this->theProtocol = prot;
    this->pcSaveFilePath = saveFilePath;
    this->pNullRezept->Name[0]='\0';

    this->TextCam1Xaxis =(char*)"Cam1 X-Axis";
    this->TextCam2Xaxis = (char*)"Cam2 X-Axis";
    this->TextZaxis = (char*)"Z-Axis";

    this->iFaktorZAchse = Rezept::DefaultWalze;

    //wird in convertCamPos benötigt
    if(sdlw/2>camw)
      {
	this->CamW_Sichtbar = camw;
	this->CamW_Unsichtbar = 0;
      }
    else
      {
	this->CamW_Sichtbar = camw -(camw-sdlw/2);
	this->CamW_Unsichtbar = camw-sdlw/2;
      }

    MitteCrossCam1=(camw*2-sdlw)/2 + sdlw/4;

    if(sdlw/2<camw)
      MitteCrossCam2=(camw-(camw-sdlw/2))/2;
    else//sdlw/2>camw
      MitteCrossCam2 = camw/2;

    for(unsigned int i = 0;i<LoadDialog::MaxRezeptFileLaenge;i++)
      {
	this->theRezept->Rezepte[i].cams[0].x_pos = 0;
	this->theRezept->Rezepte[i].cams[1].x_pos = 0;
	this->theRezept->Rezepte[i].cams[0].z_pos = 0;
	this->theRezept->Rezepte[i].cams[1].z_pos = 0;
	this->theRezept->Rezepte[i].cams[0].x_cross = MitteCrossCam1;
	this->theRezept->Rezepte[i].cams[1].x_cross = MitteCrossCam2;

	this->pNullRezept->Rezepte[i].cams[0].x_pos = 0;
	this->pNullRezept->Rezepte[i].cams[1].x_pos = 0;
	this->pNullRezept->Rezepte[i].cams[0].z_pos = 0;
	this->pNullRezept->Rezepte[i].cams[1].z_pos = 0;
	this->pNullRezept->Rezepte[i].cams[0].x_cross = MitteCrossCam1;
	this->pNullRezept->Rezepte[i].cams[1].x_cross = MitteCrossCam2;
	}

    MenuSet = new ButtonSettings(Globals::getFontButtonBig(),		\
				 &Globals::GlobalSDL_Color2,		\
				 Globals::GlobalUint32ColorDarkGray,	\
				 Globals::GlobalUint32ColorDarkGray);

    DialogSet = new ButtonSettings(Globals::getFontButtonBig(),		\
				   &Globals::GlobalSDL_ColorBlack,	\
				   Globals::GlobalUint32Color2,		\
				   Globals::GlobalUint32Color2);

    WerteSet = new ButtonSettings(Globals::getFontMono18(),		\
				  &Globals::GlobalSDL_ColorBlack,	\
				  Globals::GlobalUint32Color3,		\
				  Globals::GlobalUint32Color4);

    AlertSet = new ButtonSettings(Globals::getFontButtonBig(),		\
				  &Globals::GlobalSDL_ColorBlack,	\
				  Globals::GlobalUint32Color3,		\
				  Globals::GlobalUint32Color3);

    theLoadDialog = new LoadDialog(sdlw,sdlh,camw,camh,yPos,this);
    theErrorDialog = new ErrorDialog(sdlw,sdlh,camw,camh,yPos,this);
    theConfirmDialog = new ConfirmDialog(sdlw,sdlh,camw,camh,yPos,this);
    theSplashScreen = new SplashScreen(sdlw,sdlh,camw,camh,yPos,this);
    theNewDialog = new NewDialog(sdlw,sdlh,camw,camh,yPos,this);
    theCalDialog = new CalibrationDialog(sdlw,sdlh,camw,camh,yPos,this);
    theInfoDialog = new InfoDialog(sdlw,sdlh,camw,camh,yPos,this);
    theOptionsDialog = new OptionsDialog(sdlw,sdlh,camw,camh,yPos,this);
    theFadenkreuzDialog = new FadenkreuzDialog(sdlw,sdlh,camw,camh,yPos,this);
    theFileDeleteAbfrageDialog = new AbfrageDialog(sdlw,sdlh,camw,camh,yPos,this);
    theBackupDialog = new BackupDialog(sdlw,sdlh,camw,camh,yPos,this);
    theBackupMenuDialog = new BackupMenuDialog(sdlw,sdlh,camw,camh,yPos,this);
    theFlexibleErrorDialog = new FlexibleErrorDialog(sdlw,sdlh,camw,camh,yPos,this);
    theCreateBackupDialog = new CreateBackupDialog(sdlw,sdlh,camw,camh,yPos,this);
    theFlexibleInfoDialog = new FlexibleInfoDialog(sdlw,sdlh,camw,camh,yPos,this);

    theBackupOkCancelDialog = theBackupDialog->getOkCancelDialog();

    cap_cam_setCrossX(0,MitteCrossCam1);
    cap_cam_setCrossX(1,MitteCrossCam2);

    MLinks_x = sdlw/2 - 506;
    //MRechts_x = sdlw/2 + 6;

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

    //MName_y = yPos + 2*MSpace_h + 1*MZeile_h;
    MInfo_y = yPos + 4*MSpace_h + 4*MZeile_h+5;

    /*    if(M_y<=84)
      {
	//MName_w = 90;     //5*MZeile_h
	//MNameNr_w = 54;   //3*MZeile_h
	//MNameSpace_w = 61;//(1012 - (MName_w+MNameNr_w*8))/8 = 61,25
      }
    else
      {
	//MName_w = 112;     //4*MZeile_h
	//MNameNr_w = 56;    //2*MZeile_h
	//MNameSpace_w = 56; //(1012 - (MName_w+MNameNr_w*8))/8 = 56
      }
    */

    /*    MNameNr1_x = MLinks_x + MName_w + 1*MNameSpace_w + 0*MNameNr_w;
    MNameNr2_x = MLinks_x + MName_w + 2*MNameSpace_w + 1*MNameNr_w;
    MNameNr3_x = MLinks_x + MName_w + 3*MNameSpace_w + 2*MNameNr_w;
    MNameNr4_x = MLinks_x + MName_w + 4*MNameSpace_w + 3*MNameNr_w;
    MNameNr5_x = MLinks_x + MName_w + 5*MNameSpace_w + 4*MNameNr_w;
    MNameNr6_x = MLinks_x + MName_w + 6*MNameSpace_w + 5*MNameNr_w;
    MNameNr7_x = MLinks_x + MName_w + 7*MNameSpace_w + 6*MNameNr_w;
    MNameNr8_x = MLinks_x + MName_w + 8*MNameSpace_w + 7*MNameNr_w;
    */
    MInfoSpace_w = 3;
    MInfo_w = (1012-(6*MInfoSpace_w))/8;
    MInfoF1_x  = MLinks_x + 0*MInfo_w + 0*MInfoSpace_w;
    /*   MInfoF2_x  = MLinks_x + 1*MInfo_w + 1*MInfoSpace_w;
    MInfoF3_x  = MLinks_x + 2*MInfo_w + 2*MInfoSpace_w;
    MInfoF5_x  = MLinks_x + 3*MInfo_w + 3*MInfoSpace_w;
    MInfoF6_x  = MLinks_x + 4*MInfo_w + 4*MInfoSpace_w;
    MInfoF7_x  = MLinks_x + 5*MInfo_w + 5*MInfoSpace_w;
    MInfoF8_x  = MLinks_x + 6*MInfo_w + 6*MInfoSpace_w;
    MInfoF12_x = MLinks_x + 7*MInfo_w + 7*MInfoSpace_w;
    */
    thePosDialog = new PosDialog(theRezept->Name,sdlw,sdlh,camw,camh,yPos,this);

    theMenuBarSettings.Text[0]=(char *)"F1 load";
    theMenuBarSettings.Text[1]=(char *)"F2 edit";
    theMenuBarSettings.Text[2]=(char *)"F3 new";
    theMenuBarSettings.Text[3]=(char *)"F4 delete";
    theMenuBarSettings.Text[4]=(char *)"F5 prev";
    theMenuBarSettings.Text[5]=(char *)"F6 next";
    theMenuBarSettings.Text[6]=(char *)"F7 option";
    theMenuBarSettings.Text[7]=(char *)"F12 exit";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=F1_ListenerArbeitsDialog;
    theMenuBarSettings.evtFnks[1]=F2_ListenerArbeitsDialog;
    theMenuBarSettings.evtFnks[2]=F3_ListenerArbeitsDialog;
    theMenuBarSettings.evtFnks[3]=F4_ListenerArbeitsDialog;
    theMenuBarSettings.evtFnks[4]=F5_ListenerArbeitsDialog;
    theMenuBarSettings.evtFnks[5]=F6_ListenerArbeitsDialog;
    theMenuBarSettings.evtFnks[6]=F7_ListenerArbeitsDialog;
    theMenuBarSettings.evtFnks[7]=F12_ListenerArbeitsDialog;

    theMenu = new MenuBar(MInfoF1_x,MInfo_y,MZeile_h,(char*)"Recipe",	\
			  &this->theMenuBarSettings,this);

    LabelDialogName = new Label("Version ....",(sdlw/2)-((MInfo_w*2)/2),MInfo_y,MInfo_w*2, \
				MZeile_h,				\
				ArbeitsDialog::MenuSet);

    if(useGUI)
      {
      printf("guiMode true \n");

    this->pTSource = this; //EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->EvtTargetID=(char*)"ArbeitsDialog";
    this->setKeyboardUpEvtHandler(ArbeitsDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    
    this->addEvtTarget(&thePosDialog->EvtTargets);
    theMenu->addToEvtTarget(this);
    
    this->ArbeitsDialogEvtTargets.Next = this->EvtTargets.Next;//EvtTargets fuer spaeter sichern
    //this->showCalibrationDialog();
    this->showSplashScreen();
      }
    else
      {
	printf("guiMode false \n");
	this->pTSource = this; //EvtTarget Quelle setzenfür den EvtListener
	this->EvtTargetID=(char*)"ArbeitsDialog";
	this->addEvtTarget(this);
	this->showSplashScreen();
      }
  }

  unsigned short ArbeitsDialog::getCamW_Sichtbar()
  {
    return CamW_Sichtbar;
  }

  void ArbeitsDialog::showLoadDialog(unsigned int page,bool loadMode)
  {
    int ret=0;
    if(!useGUI)
      return;
    ret = theLoadDialog->readSaveDirectory(pcSaveFilePath,page);
    /*if(1==ret)//keine Files vorhanden
      {
	return;
      }
    else*/
      if(-1==ret)//Fehler
      {
	showErrorDialog((char*)"Error reading save directory");
	return;
      }
    else
      {
	this->iActiveDialog = ArbeitsDialog::LoadDialogIsActive;
	this->theLoadDialog->clearFilter();
	this->theLoadDialog->setLoadMode(loadMode);
	this->blankMenuArea();
	this->theGUI->activateScreen(theLoadDialog);
      }
  }

  void ArbeitsDialog::showBackupDialog(void)
  {
    int ret=0;
    if(!useGUI)
      return;
    ret = theBackupDialog->readUSBStickDirectory(0);
    if(1==ret)//keine Files vorhanden
      {
	return;
      }
    else if(-1==ret)//Fehler
      {
	showErrorDialog((char*)"Error reading USB Stick");
	return;
      }
    else
      {
	this->iActiveDialog = ArbeitsDialog::BackupDialogIsActive;
	//this->theLoadDialog->clearFilter();
	//this->theLoadDialog->setLoadMode(loadMode);
	this->blankMenuArea();
	this->theGUI->activateScreen(theBackupDialog);
      }
  }

  void ArbeitsDialog::showCreateBackupDialog(void)
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::CreateBackupDialogIsActive;
    this->blankMenuArea();
    theCreateBackupDialog->TextField_Name->setText((char*)"");
    this->theGUI->activateScreen(theCreateBackupDialog);
  }

  void ArbeitsDialog::showBackupMenuDialog(void)
  {
    //int ret=0;
    if(!useGUI)
      return;

    this->iActiveDialog = ArbeitsDialog::BackupMenuDialogIsActive;
    this->blankMenuArea();
    this->theGUI->activateScreen(theBackupMenuDialog);
  }

  void ArbeitsDialog::showArbeitsDialog()
  {
    this->iActiveDialog = ArbeitsDialog::ArbeitsDialogIsActive;
    this->blankMenuArea();
    this->theGUI->activateScreen(this);
    if(!useGUI)
      {
	this->showSplashScreen();
	return;
      }
    this->showRezept(0);
    this->sendProtocolMsg(nPEC_GET_Q1_32);
    this->sendProtocolMsg(nPEC_GET_Q2_32);
    this->sendProtocolMsg(nPEC_GET_Z1_32);
    //prt_sendmsg_uint(nPEC_GET_Z2,0x00);
  }

  void ArbeitsDialog::showErrorDialog(char * msg)
  {
    if(!useGUI)//TODO errors im BlindMode evtl. zulassen
      return;
    this->iActiveDialog = ArbeitsDialog::ErrorDialogIsActive;
    this->theErrorDialog->setErrorMsg(msg);
    this->blankMenuArea();
    this->theGUI->activateScreen(this->theErrorDialog);
  }

  void ArbeitsDialog::showConfirmDialog(char * msg)
  {
    //    if(!useGUI)//TODO errors im BlindMode evtl. zulassen
    //  return;
    this->iActiveDialog = ArbeitsDialog::ConfirmDialogIsActive;
    this->theConfirmDialog->setConfirmMsg(msg);
    this->blankMenuArea();
    this->theGUI->activateScreen(this->theConfirmDialog);
  }

  void ArbeitsDialog::showSplashScreen()
  {
    //    if(!useGUI)//TODO errors im BlindMode evtl. zulassen
    //  return;
    this->iActiveDialog = ArbeitsDialog::SplashScreenIsActive;
    this->blankMenuArea();
    this->theGUI->activateScreen(this->theSplashScreen);
  }

  void ArbeitsDialog::showInfoDialog()
  {
    //if(!useGUI)//TODO errors im BlindMode evtl. zulassen
      //return;
    this->iActiveDialog = ArbeitsDialog::InfoDialogIsActive;
    this->blankMenuArea();
    this->theInfoDialog->refreshAll();
    this->theGUI->activateScreen(this->theInfoDialog);
  }

  void ArbeitsDialog::showNewDialog(char * MenuTitel)
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::NewDialogIsActive;
    this->blankMenuArea();
    this->theGUI->activateScreen(theNewDialog);
    this->sendProtocolMsg(nPEC_GET_Q1_32);
    this->sendProtocolMsg(nPEC_GET_Q2_32);
    this->sendProtocolMsg(nPEC_GET_Z1_32);
    //this->sendProtocolMsg(nPEC_GET_Z2);
    //prt_sendmsg_uint(nPEC_GET_FP1,0x00);
    //prt_sendmsg_uint(nPEC_GET_FP2,0x00);
    theNewDialog->TextField_Name->setText(theNewDialog->tmpRezept->Name);
    //theNewDialog->Label_MenuTitle->setText(MenuTitel);
    theNewDialog->theMenu->setMenuName(MenuTitel);
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
    this->blankMenuArea();
    this->theGUI->activateScreen(theCalDialog);
  }

  void ArbeitsDialog::showOptionsDialog()
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::OptionsDialogIsActive;
    this->blankMenuArea();
    this->theGUI->activateScreen(theOptionsDialog);
  }

  void ArbeitsDialog::showFadenkreuzDialog()
  {
    if(!useGUI)
      return;
    this->iActiveDialog = ArbeitsDialog::FadenkreuzDialogIsActive;
    this->blankMenuArea();
    this->theGUI->activateScreen(theFadenkreuzDialog);
  }

  void ArbeitsDialog::showFileDeleteAbfrageDialog(char* delPath,char* fileName)
  {
    if(!useGUI)
      return;
    this->theFileDeleteAbfrageDialog->setFileToDelete(delPath,fileName);
    this->iActiveDialog = ArbeitsDialog::FileDeleteAbfrageDialogIsActive;
    this->blankMenuArea();
    this->theGUI->activateScreen(theFileDeleteAbfrageDialog);
  }

  void ArbeitsDialog::showFlexibleErrorDialog(char * msg,int DialogID)
  {
    if(!useGUI)//TODO errors im BlindMode evtl. zulassen
      return;
    this->iActiveDialog = ArbeitsDialog::FlexibleErrorDialogIsActive;
    this->theFlexibleErrorDialog->setErrorMsg(msg);
    this->theFlexibleErrorDialog->setReturnDialogID(DialogID);
    this->blankMenuArea();
    this->theGUI->activateScreen(this->theFlexibleErrorDialog);
  }

  void ArbeitsDialog::showFlexibleInfoDialog(char * msg,int DialogID)
  {
    if(!useGUI)//TODO errors im BlindMode evtl. zulassen
      return;
    this->iActiveDialog = ArbeitsDialog::FlexibleInfoDialogIsActive;
    this->theFlexibleInfoDialog->setErrorMsg(msg);
    this->theFlexibleInfoDialog->setReturnDialogID(DialogID);
    this->blankMenuArea();
    this->theGUI->activateScreen(this->theFlexibleInfoDialog);
  }

  void ArbeitsDialog::showBackupOkCancelDialog()
  {
    if(!useGUI)//TODO errors im BlindMode evtl. zulassen
      return;
    this->iActiveDialog = ArbeitsDialog::BackupOkCancelDialogIsActive;
    this->blankMenuArea();
    this->theGUI->activateScreen(this->theBackupOkCancelDialog);
  }

  SplashScreen * ArbeitsDialog::getSplashScreen()
  {
    return this->theSplashScreen;
  }

  int ArbeitsDialog::theActiveDialogNumber()
  {
    return this->iActiveDialog;
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
     //SDL_Flip(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::blankButton(Button * b)
  {
    SDL_Rect tmpRect;
    tmpRect.x = b->PosDimRect.x;
    tmpRect.y = b->PosDimRect.y;
    tmpRect.w = b->PosDimRect.w;
    tmpRect.h = b->PosDimRect.h;

    Tool::blankSurface(this->theGUI->getMainSurface(),	\
		       FSG_BACKGROUND,			\
		       &tmpRect);//TODO Rückgabewert

    SDL_UpdateRect(this->theGUI->getMainSurface(),	\
		   tmpRect.x,			\
		   tmpRect.y,			\
		   tmpRect.w,			\
		   tmpRect.h);
    //SDL_Flip(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::incRezeptNummer()
  {
    //this->Z1_evt(32768);
    this->RezeptNummer++;
    if(this->RezeptNummer>=Rezept::AnzahlRezepte)
      this->RezeptNummer = Rezept::AnzahlRezepte-1;
  }

  void ArbeitsDialog::decRezeptNummer()
  {
    //this->Z1_evt(10000);
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
    float showVal;
    Cam1XaxisCur = val;
    Cam1XaxisDif = Cam1XaxisCur - this->theRezept->getXPosition(0,RezeptNummer);
    thePosDialog->showDifferenceCam1(val,this->theRezept->getXPosition(0,RezeptNummer));
    showVal = this->convertMBProtData(val);

    sprintf(this->thePosDialog->pcLabelCam1[PosDialog::iCurr],"%7.2f mm",showVal);
    thePosDialog->pLabelCam1[PosDialog::iCurr]->		\
      setText(thePosDialog->pcLabelCam1[PosDialog::iCurr]);
    thePosDialog->pLabelCam1[PosDialog::iCurr]->show(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::setCam2XaxisCur(int val)
  {
    float showVal;
    Cam2XaxisCur = val;
    Cam2XaxisDif = Cam2XaxisCur - this->theRezept->getXPosition(1,RezeptNummer);
    thePosDialog->showDifferenceCam2(val,this->theRezept->getXPosition(1,RezeptNummer));
    showVal = this->convertMBProtData(val);

    sprintf(this->thePosDialog->pcLabelCam2[PosDialog::iCurr],"%7.2f mm",showVal);
    thePosDialog->pLabelCam2[PosDialog::iCurr]->		\
      setText(thePosDialog->pcLabelCam2[PosDialog::iCurr]);
    thePosDialog->pLabelCam2[PosDialog::iCurr]->show(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::setCam1ZaxisCur(int val,char*suffix,unsigned int zfaktor)
  {
    float showVal,ftmp;
    //printf("setCam1ZaxisCur val %i\n",val);
    ftmp = this->convertMBProtData(val,zfaktor);
    ftmp = ftmp*100;

    Cam1ZaxisCur = val;
    Cam1ZaxisDif = Cam1ZaxisCur - this->theRezept->getZPosition(RezeptNummer);
    //printf("Cam1ZaxisDif = %i\n",Cam1ZaxisDif);
    thePosDialog->showDifferenceZ((unsigned int)ftmp,this->theRezept->getZPosition(RezeptNummer));
    showVal = this->convertMBProtData(val,this->iFaktorZAchse);

    sprintf(this->thePosDialog->pcLabelZ[PosDialog::iCurr],"%7.2f%s",	\
	    showVal,					\
	    suffix);
    thePosDialog->pLabelZ[PosDialog::iCurr]->		\
      setText(thePosDialog->pcLabelZ[PosDialog::iCurr]);
    thePosDialog->pLabelZ[PosDialog::iCurr]->show(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::setCam2ZaxisCur(int val,char*suffix,unsigned int FaktorZAchse)
  {
    //nur eine Z-Achse vorhanden
    //printf("ArbeitsDialog::setCam2ZaxisCur(int val)\n");
  }

  void ArbeitsDialog::showRezept(int nummer)
  {
    if(theRezept)
      {
	RezeptNummer = nummer;

	//walze ist nur in Rezepte[0].cams[0].walze aktuell!!!
	this->iFaktorZAchse = theRezept->Rezepte[0].cams[0].walze;

	cap_cam_setCrossX(0,theRezept->Rezepte[RezeptNummer].cams[0].x_cross);
	cap_cam_setCrossX(1,theRezept->Rezepte[RezeptNummer].cams[1].x_cross);

	thePosDialog->showRecipeName(theRezept->Name);

	sprintf(thePosDialog->pcLabelCam1[PosDialog::iStep],	\
		"%7.2f mm",(float)theRezept->getXPosition(0,nummer)/100);
	thePosDialog->pLabelCam1[PosDialog::iStep]->		\
	  setText(thePosDialog->pcLabelCam1[PosDialog::iStep]);

	sprintf(thePosDialog->pcLabelCam2[PosDialog::iStep],	\
		"%7.2f mm",(float)theRezept->getXPosition(1,nummer)/100);
	thePosDialog->pLabelCam2[PosDialog::iStep]->	\
	  setText(thePosDialog->pcLabelCam2[PosDialog::iStep]);

	sprintf(thePosDialog->pcLabelZ[PosDialog::iStep],		\
		"%7.2f mm",(float)theRezept->getZPosition(nummer)/100/**this->getFaktorZAchse()*/);

	thePosDialog->pLabelZ[PosDialog::iStep]->	\
	  setText(thePosDialog->pcLabelZ[PosDialog::iStep]);

	//show Diameter
	this->newCircleDiameterFnk(theRezept->Rezepte[RezeptNummer].cams[0].diameter);

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
	this->setCam1ZaxisCur(this->Cam1ZaxisCur,(char*)(" mm"),this->iFaktorZAchse);
	this->setCam2ZaxisCur(this->Cam2ZaxisCur,(char*)(" mm"),this->iFaktorZAchse);
      }
  }

  char * ArbeitsDialog::int2string(char * tar,int len,int data,char * suffix)
  {
    float showValue;

    showValue = this->convertMBProtData(data);
    if(suffix)
      {
	snprintf(tar,len,"%7.2f%s",showValue,suffix);
      }
    else
      {
	snprintf(tar,len,"%7.2f",showValue);
      }
    return tar;
  }

  char * ArbeitsDialog::int2string(char * tar,int len,int data,char * suffix,unsigned int zFaktor)
  {
    float showValue;

    showValue = this->convertMBProtData(data,(float)zFaktor);
    if(suffix)
      {
	snprintf(tar,len,"%7.2f%s",showValue,suffix);
      }
    else
      {
	snprintf(tar,len,"%7.2f",showValue);
      }
    return tar;
  }

  float ArbeitsDialog::convertMBProtData(unsigned int dat)
  {
    float ret = 0.0;
    int i = (int)dat;
    ret = (float)i/100;

    return ret;
  }

  //Faktor ist der Durchmesser (F11 sleeve)
  //Umfang = d*PI
  //MBruch gibt mir 65535 Werte pro Umfang
  //Strecke = MBruchsWerte * (Faktor*PI)/(65536*2 wegen Michaels Interface)
  //
  float ArbeitsDialog::convertMBProtData(unsigned int dat,float faktor)
  {
    float ret = 0.0;
    int i = (int)dat;
    ret = (float)i;
    ret = ret * ((faktor/100)*3.14159265359 )/(65536*2);
    //printf("convertMBProtData %f\n",faktor);
    return ret;
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

  void ArbeitsDialog::Q1_evt(unsigned int dat)
  {
    if(iActiveDialog==ArbeitsDialog::ArbeitsDialogIsActive)
      setCam1XaxisCur(dat);
    else if(iActiveDialog==ArbeitsDialog::CalDialogIsActive)
      this->theCalDialog->setQ1(dat);
    else if(iActiveDialog==ArbeitsDialog::NewDialogIsActive)
      {
	this->theNewDialog->setNewPositionValue(NewDialog::iPosX1,	\
						dat,			\
						(char*)" mm");
      }
  }

  void ArbeitsDialog::Q2_evt(unsigned int dat)
  {
    if(iActiveDialog==ArbeitsDialog::ArbeitsDialogIsActive)
      setCam2XaxisCur(dat);
    else if(iActiveDialog==ArbeitsDialog::CalDialogIsActive)
      this->theCalDialog->setQ2(dat);
    else if(iActiveDialog==ArbeitsDialog::NewDialogIsActive)
      {
	this->theNewDialog->setNewPositionValue(NewDialog::iPosX2,	\
						dat,			\
						(char*)" mm");
      }
  }
  void ArbeitsDialog::Z1_evt(unsigned int dat)
  {
    if(iActiveDialog==ArbeitsDialog::ArbeitsDialogIsActive)
      setCam1ZaxisCur(dat,(char*)" mm",this->iFaktorZAchse);
    else if(iActiveDialog==ArbeitsDialog::CalDialogIsActive)
      this->theCalDialog->setZ1(dat,(char*)" mm",this->iFaktorZAchse);
    else if(iActiveDialog==ArbeitsDialog::NewDialogIsActive)
      {
	this->theNewDialog->setNewPositionValue(NewDialog::iPosZ,	\
						dat,			\
						(char*)" mm",this->iFaktorZAchse);
      }
  }
  void ArbeitsDialog::Z2_evt(unsigned int dat)
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
    int wert,camera;
    double step =  dat;
    if(cam) //Anzeigebereich von 0-512
      {
	camera = 1;
	wert = (int)step;
	
      }
    else   //Anzeigebereich von 288-800
      {
	camera = 0;
	wert = (int)step+CamW_Unsichtbar;
      }
    if(wert%2)  //nur duch 2 teilbar zulassen
      {
	wert--;
	//printf("camera : %i , Wert: %i\n",camera,wert);
      }
    cap_cam_setCrossX(cam,wert);
    //printf("cam:%i dat:%i wert:%i\n",camera,dat,wert);
  }
  void ArbeitsDialog::FP1_evt(unsigned int dat)
  {
    convertCamPos(0,dat);
    theNewDialog->getCam1CrossX();//hier werden die cross Daten zum abspeichern geholt
    //printf("ArbeitsDialog getFP1:%i\n",dat);
    this->theSplashScreen->showCrossPosition(0,dat);
  }
  void ArbeitsDialog::FP2_evt(unsigned int dat)
  {
    convertCamPos(2,dat);
    theNewDialog->getCam2CrossX();//hier werden die cross Daten zum abspeichern geholt
    //printf("ArbeitsDialog getFB2:%i\n",dat);
    this->theSplashScreen->showCrossPosition(1,dat);
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

  void ArbeitsDialog::sendProtocolMsg(char cmd,int val)
  {
    if(theProtocol)
      if(theProtocol->isInitialised())
	prt_sendmsg_uint(cmd,val);
  }

  void ArbeitsDialog::setFaktorZAchse(int i)
  {
    this->iFaktorZAchse = i;
  }

  int ArbeitsDialog::getFaktorZAchse(void)
  {
    return this->iFaktorZAchse;
  }

  bool ArbeitsDialog::useTheGUI()
  {
    return this->useGUI;
  }

  void ArbeitsDialog::clearFilter(void)
  {
    theLoadDialog->clearFilter();
  }

  BackupMenuDialog * ArbeitsDialog::getBackupMenuDialog(void)
  {
    return this->theBackupMenuDialog;
  }
}//end Namespace
