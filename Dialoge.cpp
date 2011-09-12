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
    if(ActualStep>CalibrationDialog::iZ2)
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
		   "Move camera 1 Z-Axis in zero position :");
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

static void evtB1(void * src,SDL_Event * evt){
  cap_cam_addCrossX(0,-10);
}
static void evtB2(void * src,SDL_Event * evt){
  cap_cam_addCrossX(0,-2);
}
static void evtB3(void * src,SDL_Event * evt){
  cap_cam_addCrossX(0,2);
}
static void evtB4(void * src,SDL_Event * evt){
  cap_cam_addCrossX(0,10);
}
static void evtB5(void * src,SDL_Event * evt){
  cap_cam_addCrossX(1,-10);
}
static void evtB6(void * src,SDL_Event * evt){
  cap_cam_addCrossX(1,-2);
}
static void evtB7(void * src,SDL_Event * evt){
  cap_cam_addCrossX(1,2);
}
static void evtB8(void * src,SDL_Event * evt){
  cap_cam_addCrossX(1,10);
}
static void evtExit(void * src,SDL_Event * evt){
  GUI::getInstance(0,0)->stopEventLoop();
}

  MainDialog::MainDialog(int sdlwidth, \
			 int sdlheight, \
			 int camwidth,\
			 int camheight,\
			 int yPos):Screen()
  {
    yPos = yPos +5;

/*
<------------------| sdlwidth/2

<------------- camwidth*2 ---------------->

________________________________________
|                  |                   |
|                  |                   |
|                  |                   |
|                  |                   |
|                  |                   |
|                  |                   |
|__________________|___________________|
|                                      |
|                                      |
|                                      |
|                                      |

         | camhalbe0
           camhalbe1          | 

camhalbe0 =  sdlwidth/2 -camwidth/2
camhalbe1 =  sdlwidth/2 +camwidth/2

    B1 B2 B3 B4   Bexit  B5 B6 B7 B8   
    << < | > >>          << < | > >>

B1 = camhalbe0 - 2*Buttonwidth - 2*Abstand
B2 = camhalbe0 - 1*Buttonwidth - 1*Abstand
B3 = camhalbe0 + 0*Buttonwidth + 1*Abstand
B4 = camhalbe0 + 1*Buttonwidth + 2*Abstand
B5 = camhalbe1 - 2*ButtonWidth - 2*Abstand
B6 = camhalbe1 - 1*            - 1*Abstand
B7 = camhalbe1 + 0*            + 1*
B8 = camhalbe1 + 1*            + 2*
Bexit = sdlw/2 - Buttonwidth/2  
*/
  int sdlw = sdlwidth;
  int camhalbe0 = sdlw/2 - camwidth/2;
  int camhalbe1 = sdlw/2 + camwidth/2;
  int X = 0;
  int Y = yPos; //camheight + 20;//hier fangen die Buttons an
  int BtnW=60;
  int BtnH=30;
  int Abstand = 5;

  SDL_Rect PosDimRect={0+X,0+Y,BtnW,BtnH};

  PosDimRect.x = camhalbe0 - 2*BtnW - 2*Abstand;
  B1=new Button("<<",PosDimRect);
  B1->setLMButtonUpEvtHandler(evtB1);

  PosDimRect.x = camhalbe0 - 1*BtnW - 1*Abstand;
  B2=new Button("<",PosDimRect);
  B2->setLMButtonUpEvtHandler(evtB2);

  PosDimRect.x = camhalbe0 + 0*BtnW + 1*Abstand;
  this->B3=new Button(">",PosDimRect);
  this->B3->setLMButtonUpEvtHandler(evtB3);

  PosDimRect.x = camhalbe0 + 1*BtnW + 2*Abstand;
  this->B4=new Button(">>",PosDimRect);
  this->B4->setLMButtonUpEvtHandler(evtB4);

  PosDimRect.x = camhalbe1 - 2*BtnW - 2*Abstand;
  this->B5=new Button("<<",PosDimRect);
  this->B5->setLMButtonUpEvtHandler(evtB5);

  PosDimRect.x = camhalbe1 - 1*BtnW - 1*Abstand;
  this->B6=new Button("<",PosDimRect);
  this->B6->setLMButtonUpEvtHandler(evtB6);

  PosDimRect.x = camhalbe1 + 0*BtnW + 1*Abstand;
  this->B7=new Button(">",PosDimRect);
  this->B7->setLMButtonUpEvtHandler(evtB7);

  PosDimRect.x = camhalbe1 + 1*BtnW + 2*Abstand;
  this->B8=new Button(">>",PosDimRect);
  this->B8->setLMButtonUpEvtHandler(evtB8);
  
  PosDimRect.x = sdlw/2 - BtnW/2;
  this->Bexit=new Button("QUIT",PosDimRect);
  this->Bexit->setLMButtonUpEvtHandler(evtExit);  

  addEvtTarget(B1);
  addEvtTarget(B2);
  addEvtTarget(B3);
  addEvtTarget(B4);
  addEvtTarget(B5);
  addEvtTarget(B6);
  addEvtTarget(B7);
  addEvtTarget(B8);
  addEvtTarget(Bexit);

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
	    prt_sendmsg_uint(nPEC_RESET_Q1,0x00);
	    prt_sendmsg_uint(nPEC_GET_Q1,0x00);
	  }
	else if(key->keysym.sym == SDLK_F3)
	  {
	    ad->showNewDialog();//printf("F3\n");
	  }
	else if(key->keysym.sym == SDLK_F4)
	  {
	    printf("F4\n");
	  }
	else if(key->keysym.sym == SDLK_F5)
	  {
	    ad->decRezeptNummer();
	    ad->showRezept(ad->getRezeptNummer());
	    /* int xxx=0;
	    xxx = cap_cam_getCrossX(0);
	    cap_cam_setCrossX(0,xxx-10);
	    ad->setCross1Ref();
	    xxx = cap_cam_getCrossX(1);
	    cap_cam_setCrossX(1,xxx-10);
	    ad->setCross2Ref();*/
	  }
	else if(key->keysym.sym == SDLK_F6)
	  {
	    ad->incRezeptNummer();
	    ad->showRezept(ad->getRezeptNummer());
	    //	    int xx=0;
	    //xx = cap_cam_getCrossX(0);
	    //cap_cam_setCrossX(0,xx+10);
	    ad->setCross1Ref();
	    //xx = cap_cam_getCrossX(1);
	    //cap_cam_setCrossX(1,xx+10);
	    ad->setCross2Ref();
	  }
	else if(key->keysym.sym == SDLK_F7)
	  {
	    ad->showCalibrationDialog();
	  }
	else if(key->keysym.sym == SDLK_F12)
	  {
	    exit(0);
	  }
	/*	else
	  {
	    zeichen = Tool::getStdASCII_Char(key);
	  }
	if(zeichen)
	  {
	    tf->addChar(zeichen);
	    }*/
      }
  }
  ArbeitsDialog::~ArbeitsDialog(){}
  ArbeitsDialog::ArbeitsDialog(GUI * pGUI,	\
			       MBProtocol *prot,\
			       int sdlw,	\
			       int sdlh,	\
			       int camw,	\
			       int camh,	\
			       int yPos)//:Screen()
  {
    /*    this->KeyListener = new EvtTarget();
    if(this->KeyListern)
      KeyListern->setKeyboardUpEvtHandler(void (*pfnk)(void * src,SDL_Event *))
	{
	  this->fnkKeyboardUp = pfnk;
	}
    */

/*
<------------------| sdlwidth/2

___________________________________________
|                  |                      |
|                  |                      |
|                  |                      |
|                  |                      |
|                  |                      |
|                  |                      |
|__________________|______________________|


*/
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

    this->theRezept = new Rezept();
    this->theGUI = pGUI;
    this->theProtocol = prot;
    theLoadDialog = new LoadDialog(sdlw,sdlh,camw,camh,yPos,this);
    theErrorDialog = new ErrorDialog(sdlw,sdlh,camw,camh,yPos,this);
    theNewDialog = new NewDialog(sdlw,sdlh,camw,camh,yPos,this);
    theCalDialog = new CalibrationDialog(sdlw,sdlh,camw,camh,yPos,this);

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
    MInfo_y = yPos + 4*MSpace_h + 4*MZeile_h;

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

    Pos_Cam1 = new PositionDialog("CAM 1",sdlw/2-506,MName_y,\
				  506,1*MSpace_h+3*MZeile_h);
    Pos_Cam2 = new PositionDialog("CAM 2",sdlw/2,MName_y,\
				  506,1*MSpace_h+3*MZeile_h);

    Label_RezeptName = new Label("----------------",MLinks_x,yPos+MSpace_h,\
				 MName_w,MZeile_h);
    Label_RezeptNr1 = new Label("1",MNameNr1_x,yPos+MSpace_h,MNameNr_w,MZeile_h);
    Label_RezeptNr2 = new Label("2",MNameNr2_x,yPos+MSpace_h,MNameNr_w,MZeile_h);
    Label_RezeptNr3 = new Label("3",MNameNr3_x,yPos+MSpace_h,MNameNr_w,MZeile_h);
    Label_RezeptNr4 = new Label("4",MNameNr4_x,yPos+MSpace_h,MNameNr_w,MZeile_h);
    Label_RezeptNr5 = new Label("5",MNameNr5_x,yPos+MSpace_h,MNameNr_w,MZeile_h);
    Label_RezeptNr6 = new Label("6",MNameNr6_x,yPos+MSpace_h,MNameNr_w,MZeile_h);
    Label_RezeptNr7 = new Label("7",MNameNr7_x,yPos+MSpace_h,MNameNr_w,MZeile_h);
    Label_RezeptNr8 = new Label("8",MNameNr8_x,yPos+MSpace_h,MNameNr_w,MZeile_h);

    Label_InfoF1 = new Label("F1: load",MInfoF1_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF2 = new Label("F2: save",MInfoF2_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF3 = new Label("F3: new",MInfoF3_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF5 = new Label("F5: prev step",MInfoF5_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF6 = new Label("F6: next step",MInfoF6_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF7 = new Label("F7: calibration",MInfoF7_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF8 = new Label("F8:",MInfoF8_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF12 = new Label("F12:",MInfoF12_x,MInfo_y,MInfo_w,MZeile_h);

    this->pTSource = this; //EvtTarget Quelle setzen, damit der EvtListener die Quell mitteilen kann
    this->setKeyboardUpEvtHandler(ArbeitsDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!


    pLabel_CurrentRezept[0] = Label_RezeptNr1;
    pLabel_CurrentRezept[1] = Label_RezeptNr2;
    pLabel_CurrentRezept[2] = Label_RezeptNr3;
    pLabel_CurrentRezept[3] = Label_RezeptNr4;
    pLabel_CurrentRezept[4] = Label_RezeptNr5;
    pLabel_CurrentRezept[5] = Label_RezeptNr6;
    pLabel_CurrentRezept[6] = Label_RezeptNr7;
    pLabel_CurrentRezept[7] = Label_RezeptNr8;
    
    addEvtTarget(&Pos_Cam1->EvtTargets);
    addEvtTarget(&Pos_Cam2->EvtTargets);

    addEvtTarget(Label_RezeptName);
    addEvtTarget(Label_RezeptNr1);
    addEvtTarget(Label_RezeptNr2);
    addEvtTarget(Label_RezeptNr3);
    addEvtTarget(Label_RezeptNr4);
    addEvtTarget(Label_RezeptNr5);
    addEvtTarget(Label_RezeptNr6);
    addEvtTarget(Label_RezeptNr7);
    addEvtTarget(Label_RezeptNr8);
    
    addEvtTarget(Label_InfoF1);
    addEvtTarget(Label_InfoF2);
    addEvtTarget(Label_InfoF3);
    addEvtTarget(Label_InfoF5);
    addEvtTarget(Label_InfoF6);
    addEvtTarget(Label_InfoF7);
    addEvtTarget(Label_InfoF8);
    addEvtTarget(Label_InfoF12);
    
    this->ArbeitsDialogEvtTargets.Next = this->EvtTargets.Next;//EvtTargets fuer spaeter sichern
    this->showCalibrationDialog();
  }

  void ArbeitsDialog::showLoadDialog(unsigned int page)
  {
    this->iActiveDialog = ArbeitsDialog::LoadDialogIsActive;
    if(theLoadDialog->readSaveDirectory("data",page))
      {
	showErrorDialog("Error reading save directory");
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
    prt_sendmsg_uint(nPEC_GET_Q1,0x00);
    prt_sendmsg_uint(nPEC_GET_Q2,0x00);
    prt_sendmsg_uint(nPEC_GET_Z1,0x00);
    prt_sendmsg_uint(nPEC_GET_Z2,0x00);
  }

  void ArbeitsDialog::showErrorDialog(char * msg)
  {
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

  void ArbeitsDialog::showNewDialog()
  {
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
	this->theNewDialog->decStep();
  }

  void ArbeitsDialog::showCalibrationDialog()
  {
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

  /*  void ArbeitsDialog::showRezept(Rezept * pRezept,int nummer)
  {
    this->theRezept = pRezept;
    this->showRezept(nummer);
    }*/

  void ArbeitsDialog::setCam1XaxisCur(int val)
  {
    Cam1XaxisCur = val;
    Cam1XaxisDif = Cam1XaxisCur - this->theRezept->getXPosition(0,RezeptNummer);
    sprintf(this->Pos_Cam1->getXaxisDifBuf(),"%i",Cam1XaxisDif);
    Pos_Cam1->LabelXaxisDif->setText(this->Pos_Cam1->getXaxisDifBuf());
    Label::showLabel((void*)Pos_Cam1->LabelXaxisDif,this->theGUI->getMainSurface());

    sprintf(this->Pos_Cam1->getXaxisCurBuf(),"%i",Cam1XaxisCur);
    Pos_Cam1->LabelXaxisCur->setText(this->Pos_Cam1->getXaxisCurBuf());
    Label::showLabel((void*)Pos_Cam1->LabelXaxisCur,this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::setCam2XaxisCur(int val)
  {
    Cam2XaxisCur = val;
    Cam2XaxisDif = Cam2XaxisCur - this->theRezept->getXPosition(1,RezeptNummer);
    sprintf(this->Pos_Cam2->getXaxisDifBuf(),"%i",Cam2XaxisDif);
    Pos_Cam2->LabelXaxisDif->setText(this->Pos_Cam2->getXaxisDifBuf());
    Label::showLabel((void*)Pos_Cam2->LabelXaxisDif,this->theGUI->getMainSurface());
    sprintf(this->Pos_Cam2->getXaxisCurBuf(),"%i",Cam2XaxisCur);
    Pos_Cam2->LabelXaxisCur->setText(this->Pos_Cam2->getXaxisCurBuf());
    Label::showLabel((void*)Pos_Cam2->LabelXaxisCur,this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::setCam1ZaxisCur(int val)
  {
    Cam1ZaxisCur = val;
    Cam1ZaxisDif = Cam1ZaxisCur - this->theRezept->getXPosition(0,RezeptNummer);
    sprintf(this->Pos_Cam1->getZaxisDifBuf(),"%i",Cam1ZaxisDif);
    Pos_Cam1->LabelZaxisDif->setText(this->Pos_Cam1->getZaxisDifBuf());
    Label::showLabel((void*)Pos_Cam1->LabelZaxisDif,this->theGUI->getMainSurface());

    sprintf(this->Pos_Cam1->getZaxisCurBuf(),"%i",Cam1ZaxisCur);
    Pos_Cam1->LabelZaxisCur->setText(this->Pos_Cam1->getZaxisCurBuf());
    Label::showLabel((void*)Pos_Cam1->LabelZaxisCur,this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::setCam2ZaxisCur(int val)
  {
    Cam2ZaxisCur = val;
    Cam2ZaxisDif = Cam2ZaxisCur - this->theRezept->getXPosition(1,RezeptNummer);
    sprintf(this->Pos_Cam2->getZaxisDifBuf(),"%i",Cam2ZaxisDif);
    Pos_Cam2->LabelZaxisDif->setText(this->Pos_Cam2->getZaxisDifBuf());
    Label::showLabel((void*)Pos_Cam2->LabelZaxisDif,this->theGUI->getMainSurface());
    sprintf(this->Pos_Cam2->getZaxisCurBuf(),"%i",Cam2ZaxisCur);
    Pos_Cam2->LabelZaxisCur->setText(this->Pos_Cam2->getZaxisCurBuf());
    Label::showLabel((void*)Pos_Cam2->LabelZaxisCur,this->theGUI->getMainSurface());
  }

  /*  void ArbeitsDialog::setCam1Cur_Cross(int val)
  {
    Cam1Cur_Cross = val;
    Cam1Dif_Cross = Cam1Cur_Cross - this->theRezept->getCrossPosition(0,RezeptNummer);
    sprintf(this->Pos_Cam1->getCrossDifBuf(),"%i",Cross1Dif);
    Pos_Cam2->Label_WertDif->setText(this->Pos_Cam2->getXaxisDifBuf());
    Label::showLabel((void*)Pos_Cam2->Label_WertDif,this->theGUI->getMainSurface());
    sprintf(this->Pos_Cam2->getXaxisCurBuf(),"%i",Cam2Cur);
    Pos_Cam2->Label_WertCur->setText(this->Pos_Cam2->getXaxisCurBuf());
    Label::showLabel((void*)Pos_Cam2->Label_WertCur,this->theGUI->getMainSurface());
  }
  void ArbeitsDialog::setCam2Cur_Cross(int val)
  {
    Cam2Cur = val;
    Cam2Dif = Cam2Cur - this->theRezept->getXPosition(1,RezeptNummer);
    sprintf(this->Pos_Cam2->getXaxisDifBuf(),"%i",Cam2Dif);
    Pos_Cam2->Label_WertDif->setText(this->Pos_Cam2->getXaxisDifBuf());
    Label::showLabel((void*)Pos_Cam2->Label_WertDif,this->theGUI->getMainSurface());
    sprintf(this->Pos_Cam2->getXaxisCurBuf(),"%i",Cam2Cur);
    Pos_Cam2->Label_WertCur->setText(this->Pos_Cam2->getXaxisCurBuf());
    Label::showLabel((void*)Pos_Cam2->Label_WertCur,this->theGUI->getMainSurface());
    }*/

  void ArbeitsDialog::showRezept(int nummer)
  {
    if(theRezept)
      {
	RezeptNummer = nummer;
	sprintf(this->Pos_Cam1->getXaxisRefBuf(),"%i",theRezept->getXPosition(0,nummer));
	Pos_Cam1->LabelXaxisRef->setText(this->Pos_Cam1->getXaxisRefBuf());
	sprintf(this->Pos_Cam2->getXaxisRefBuf(),"%i",theRezept->getXPosition(1,nummer));
	Pos_Cam2->LabelXaxisRef->setText(this->Pos_Cam2->getXaxisRefBuf());

	//Difference Berechnen
	this->setCam1XaxisCur(this->Cam1XaxisCur);
	this->setCam2XaxisCur(this->Cam2XaxisCur);
	this->setCam1ZaxisCur(this->Cam1ZaxisCur);
	this->setCam2ZaxisCur(this->Cam2ZaxisCur);

	for(int i=0;i<Rezept::AnzahlRezepte;i++)
	  pLabel_CurrentRezept[i]->setBorder(false);

	pLabel_CurrentRezept[RezeptNummer]->setBorder(true);
      }
  }

  char * ArbeitsDialog::int2string(char * tar,int len,int data)
  {
    snprintf(tar,len,"%i",data);
    return tar;
  }
  
  void ArbeitsDialog::setCross1Ref()
  {
    sprintf(this->Pos_Cam1->getCrossRefBuf(),"%i",cap_cam_getCrossX(0));
    Pos_Cam1->Label_CrossRef->setText(this->Pos_Cam1->getCrossRefBuf());
  }

  void ArbeitsDialog::setCross2Ref()
  {
    sprintf(this->Pos_Cam2->getCrossRefBuf(),"%i",cap_cam_getCrossX(1));
    Pos_Cam2->Label_CrossRef->setText(this->Pos_Cam2->getCrossRefBuf());
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
	this->theNewDialog->LabelWerte[0]->setText(int2string(theNewDialog->pcWerte[0],	\
							      64,(int)dat));
	Label::showLabel((void*)theNewDialog->LabelWerte[0],this->theGUI->getMainSurface());
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
	this->theNewDialog->LabelWerte[3]->setText(int2string(theNewDialog->pcWerte[3],	\
							      64,(int)dat));
	Label::showLabel((void*)theNewDialog->LabelWerte[3],this->theGUI->getMainSurface());
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
	this->theNewDialog->LabelWerte[1]->setText(int2string(theNewDialog->pcWerte[1],	\
							      64,(int)dat));
	Label::showLabel((void*)theNewDialog->LabelWerte[1],this->theGUI->getMainSurface());
      }
  }
  void ArbeitsDialog::Z2_evt(unsigned short dat)
  {
    if(iActiveDialog==ArbeitsDialog::ArbeitsDialogIsActive)
      setCam2ZaxisCur(dat);
    else if(iActiveDialog==ArbeitsDialog::CalDialogIsActive)
      this->theCalDialog->setZ2(dat);
    else if(iActiveDialog==ArbeitsDialog::NewDialogIsActive)
      {
	this->theNewDialog->LabelWerte[4]->setText(int2string(theNewDialog->pcWerte[4],	\
							      64,(int)dat));
	Label::showLabel((void*)theNewDialog->LabelWerte[4],this->theGUI->getMainSurface());
      }
  }
  void ArbeitsDialog::FP1_evt(unsigned short dat)
  {
    printf("ArbeitsDialog getFP1:%i\n",dat);
  }
  void ArbeitsDialog::FP2_evt(unsigned short dat)
  {
    printf("ArbeitsDialog getFB2:%i\n",dat);
  }
  void ArbeitsDialog::SWVersion_evt(unsigned short dat)
  {
    printf("ArbeitsDialog SWVersion:%i\n",dat);
  }
  void ArbeitsDialog::HWVersion_evt(unsigned short dat)
  {
    printf("ArbeitsDialog HWVersion:%i\n",dat);
  }

  char * PositionDialog::getXaxisCurBuf()
  {
    return this->XaxisCurBuf;
  }
  char * PositionDialog::getXaxisDifBuf()
  {
    return this->XaxisDifBuf;
  }
  char * PositionDialog::getXaxisRefBuf()
  {
    return this->XaxisRefBuf;
  }
  char * PositionDialog::getZaxisCurBuf()
  {
    return this->ZaxisCurBuf;
  }
  char * PositionDialog::getZaxisDifBuf()
  {
    return this->ZaxisDifBuf;
  }
  char * PositionDialog::getZaxisRefBuf()
  {
    return this->ZaxisRefBuf;
  }
  char * PositionDialog::getCrossRefBuf()
  {
    return this->CrossRefBuf;
  }
  char * PositionDialog::getCrossCurBuf()
  {
    return this->CrossCurBuf;
  }

  PositionDialog::PositionDialog(char * text,			\
				 short x,			\
				 short y,			\
				 unsigned short w,		\
				 unsigned short h):Screen()
  {
    /*      
      x/y    
    |<-------------------- w ---------------->|
                                                VSpace
    |s |A |s|  B1     |s| B2      |s| B3      |  LineH
                                                VSpace
    |  |    |  WertB1 | | WertB2  | | WertB3  |  LineH 


      s = HSpace = 3 Pixel

     */
    
    //unsigned short Aw;
    short B1x,B2x,B3x,B4x;
    short Y1,Y2,Y3,Y4;
    unsigned short Bw;
    unsigned short HSpace,VSpace;
    unsigned short LineH;
    int i;
    for(i=0;i<64;i++)
      {
	if(i<=3)
	  {
	    XaxisCurBuf[i] = '-';
	    XaxisDifBuf[i] = '-';
	    XaxisRefBuf[i] = '-';
	    ZaxisCurBuf[i] = '-';
	    ZaxisDifBuf[i] = '-';
	    ZaxisRefBuf[i] = '-';
	    CrossCurBuf[i] = '-';
	    CrossRefBuf[i] = '-';
	  }
	else
	  {
	    XaxisCurBuf[i] = '\0';
	    XaxisDifBuf[i] = '\0';
	    XaxisRefBuf[i] = '\0';
	    ZaxisCurBuf[i] = '\0';
	    ZaxisDifBuf[i] = '\0';
	    ZaxisRefBuf[i] = '\0';
	    CrossCurBuf[i] = '\0';
	    CrossRefBuf[i] = '\0';
	  }
      }
    HSpace = 3;
    VSpace = 3;
    Y1  = y+VSpace;
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
    Y4 = Y3 + LineH + VSpace;

    LabelName = new Label(text,B1x,Y1,Bw,LineH);

    LabelNameRef = new Label("reference",B1x,Y2,Bw,LineH);
    LabelNameCur = new Label("current",B1x,Y3,Bw,LineH);
    LabelNameDif = new Label("difference",B1x,Y4,Bw,LineH);

    Label_CrossPos = new Label("Crossair",B4x,Y1,Bw,LineH);
    Label_CrossCur = new Label(CrossCurBuf,B4x,Y3,Bw,LineH);
    Label_CrossRef = new Label(CrossRefBuf,B4x,Y2,Bw,LineH);

    LabelXaxis = new Label("X - Axis",B2x,Y1,Bw,LineH);
    LabelXaxisRef = new Label(XaxisRefBuf,B2x,Y2,Bw,LineH);
    LabelXaxisCur = new Label(XaxisCurBuf,B2x,Y3,Bw,LineH);
    LabelXaxisDif = new Label(XaxisDifBuf,B2x,Y4,Bw,LineH);

    LabelZaxis = new Label("Y - Axis",B3x,Y1,Bw,LineH);
    LabelZaxisRef = new Label(ZaxisRefBuf,B3x,Y2,Bw,LineH);
    LabelZaxisCur = new Label(ZaxisCurBuf,B3x,Y3,Bw,LineH);
    LabelZaxisDif = new Label(ZaxisDifBuf,B3x,Y4,Bw,LineH);
 
    
    addEvtTarget(LabelName);
    addEvtTarget(LabelNameCur);
    addEvtTarget(LabelNameDif);
    addEvtTarget(LabelNameRef);

    addEvtTarget(Label_CrossPos);
    addEvtTarget(Label_CrossCur);
    addEvtTarget(Label_CrossRef);

    addEvtTarget(LabelXaxis);
    addEvtTarget(LabelXaxisCur);
    addEvtTarget(LabelXaxisDif);
    addEvtTarget(LabelXaxisRef);

    addEvtTarget(LabelZaxis);
    addEvtTarget(LabelZaxisCur);
    addEvtTarget(LabelZaxisDif);
    addEvtTarget(LabelZaxisRef);
  };

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
    short MLoadName_y, MLabels_y;

    this->Parent = parent;
    this->ActiveRecipe = 0;
    this->MaxRecipesToDisplay = 0;
    this->ActiveSavePage = 0;
    this->MaxSavePages = 0;

    M_y = sdlh - yPos;
    //[master cec6470] Dialoge: get und set Crossaire
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

    MLoadName_y  = yPos + 1*MSpace_h + 0*MZeile_h;
    Rezepte_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Rezepte_w = 108;

    Label_LadenName = new Label("LOAD RECIPE",MLinks_x,MLoadName_y,506*2,MZeile_h);
    int ii = 0;
    char tmpc[16] = { 'x','X','x','x','x','x','x','x'};
    tmpc[15]='\0';
    for(int i=0;i<LoadDialog::RezepteLen;i++)
      {
	if(ii>=LoadDialog::RezepteProZeile)
	  {
	    ii=0;
	    Rezepte_y += 1*MSpace_h + 1*MZeile_h;
	  }
	sprintf(DateiNamen[i],"%s",tmpc);                   //Text Buffer füllen
	pLabel_Rezepte[i] = new Label(DateiNamen[i],			\
				      MLinks_x + ii*MSpace_h + ii*Rezepte_w, \
				      Rezepte_y,			\
				      Rezepte_w,			\
				      MZeile_h);
	ii++;
	addEvtTarget(pLabel_Rezepte[i]);
      }
    setActiveRecipe(10);

    addEvtTarget(Label_LadenName);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(LoadDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!

    //MLabels_y = yPos + 2*MSpace_h + 1*MZeile_h;
  }

  /* \brief Makiert das aktive Label und desmaskiert den Vorherigen.
   */ 
  void LoadDialog::setActiveRecipe(int nr)
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
    int n;
    unsigned int fileToShow;

    this->EvtTargets.Next = 0;     //alle FileLabels deaktivieren
    this->Next = 0;                //alles nach dem Keylistener = 0
    this->addEvtTarget(this);      //den Screen Key Listener bei sich selber anmelden!
    this->Label_LadenName->Next = 0; //Laden Label anzeigen
    this->addEvtTarget(Label_LadenName);
    this->ActiveSavePage = page;

    n = scandir(dirName, &namelist, dirFilter, alphasort);
    if (n < 0)
      return -1;

    int ii = n;
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
	int i = 0;
	int i2 = 0;
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
    this->Parent->showErrorDialog("Cannot open Save File Directory");
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
  
  static char * NewDialogMainMenuText = "MainMenu :   RETURN : save recipe | ESC : abort | " \
    "LEFT previous step | RIGHT next step | F12 Crossaire Menu";
  static char * NewDialogCrossMenuText = "Crossaire Menu: "	\
    "F1: Cam1 << | F2: Cam1 < | F3: Cam1 > | F4: Cam1 >> | "	\
    "F5: Cam2 << | F6: Cam2 < | F7: Cam2 > | F8: Cam2 >> | ";
  
  static void NewDialogKeyListener(void * src, SDL_Event * evt)
  {
    NewDialog* ad = (NewDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    char zeichen = 0;
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
		printf("Rezepte fertig! abspeicher!!!\n");
		ad->Parent->showArbeitsDialog();
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
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y;

    this->Parent = parent;
    this->step = 0;
    this->theMenuModus = iMainMenu;
    this->theRecipe.Name[0]='\0';
    for(int i = 0;i<LoadDialog::MaxRezeptFileLaenge;i++)
      {
	this->theRecipe.Rezepte[i].cams[0].x_pos = 0;
	this->theRecipe.Rezepte[i].cams[1].x_pos = 0;
	this->theRecipe.Rezepte[i].cams[0].z_pos = 0;
	this->theRecipe.Rezepte[i].cams[1].z_pos = 0;
      }

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

    /*if(M_y<=84)
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
       }*/

    MLinks_x = sdlw/2 - 506;

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    //Label_NewName = new Label("NEW RECIPE",MLinks_x,Zeile1_y,506*2,MZeile_h);
    Label_Name = new Label("NEW RECIPE     Name :",MLinks_x,Zeile1_y,506-MSpace_h,MZeile_h);
    Label_Name->setFont(Globals::getFontButtonBig());

    Label_Info = new Label("Recipe Name",MLinks_x,Zeile5_y,150,MZeile_h);
    Label_Info->setFont(Globals::getFontButtonBig());
    snprintf(this->InfoText,64,"Recipe Name");
    this->Label_Info->setText(this->InfoText);

    /*    Label_Menu = new Label("RETURN : save recipe | ESC : abort | " \
			   "LEFT previous step | RIGHT next step",	\
			   MLinks_x+156,Zeile5_y,1012-156,MZeile_h);
    */
    Label_Menu = new Label(NewDialogMainMenuText,			\
			   MLinks_x+156,Zeile5_y,1012-156,MZeile_h);    

    TextField_Name = new TextField(0,LoadDialog::MaxRezeptFileLaenge,	\
				   MLinks_x+506+2*MSpace_h,		\
				   Zeile1_y,506-2*MSpace_h,		\
				   MZeile_h);
    TextField_Name->setFont(Globals::getFontButtonBig());
    TextField_Name->setActive(true);
    short Mitte_w = 50;
    LabelXaxisText = new Label("X-Axis",506-Mitte_w,Zeile2_y,106,MZeile_h);
    LabelZaxisText = new Label("Y-Axis",506-Mitte_w,Zeile3_y,106,MZeile_h);
    LabelCrossText = new Label("Cross",506-Mitte_w,Zeile4_y,106,MZeile_h);

    for(int i =0;i<6;i++)
      {
	pcWerte[i][0] = '-';
	pcWerte[i][1] = '-';
	pcWerte[i][2] = '-';
	pcWerte[i][3] = '\0';	
      }

    LabelWerte[0] = new Label(pcWerte[0],MLinks_x,Zeile2_y,506-Mitte_w-12,MZeile_h);
    LabelWerte[1] = new Label(pcWerte[1],MLinks_x,Zeile3_y,506-Mitte_w-12,MZeile_h);
    LabelWerte[2] = new Label(pcWerte[2],MLinks_x,Zeile4_y,506-Mitte_w-12,MZeile_h);

    LabelWerte[3] = new Label(pcWerte[3],506+Mitte_w+12,Zeile2_y,506-Mitte_w-6,MZeile_h);
    LabelWerte[4] = new Label(pcWerte[4],506+Mitte_w+12,Zeile3_y,506-Mitte_w-6,MZeile_h);
    LabelWerte[5] = new Label(pcWerte[5],506+Mitte_w+12,Zeile4_y,506-Mitte_w-6,MZeile_h);
    //    LabelWerte[0]->setText(pcWerte[1]);

    //addEvtTarget(Label_NewName);
    addEvtTarget(Label_Name);
    addEvtTarget(TextField_Name);
    addEvtTarget(Label_Info);
    addEvtTarget(Label_Menu);
    addEvtTarget(LabelXaxisText);
    addEvtTarget(LabelZaxisText);
    addEvtTarget(LabelCrossText);
    addEvtTarget(LabelWerte[0]);
    addEvtTarget(LabelWerte[1]);
    addEvtTarget(LabelWerte[2]);
    addEvtTarget(LabelWerte[3]);
    addEvtTarget(LabelWerte[4]);
    addEvtTarget(LabelWerte[5]);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(NewDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
  }

  void NewDialog::getCam1CrossX()
  {
    LabelWerte[2]->setText(Parent->int2string(pcWerte[2], \
					      64,cap_cam_getCrossX(0)));
    Label::showLabel((void*)LabelWerte[2],Parent->theGUI->getMainSurface());
  }

  void NewDialog::getCam2CrossX()
  {
    LabelWerte[5]->setText(Parent->int2string(pcWerte[5], \
					      64,cap_cam_getCrossX(1)));
    Label::showLabel((void*)LabelWerte[5],Parent->theGUI->getMainSurface());
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
	for(int i =0;i<6;i++)
	  {
	     LabelWerte[i]->setBorder(true);
	  }
      }
    getCam1CrossX();
    getCam2CrossX();
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
	for(int i =0;i<6;i++)
	  {
	    LabelWerte[i]->setBorder(true);
	  }
      }
    else
      {
	snprintf(this->InfoText,256,					\
		 "Recipe step %i",this->step);
      }
    getCam1CrossX();
    getCam2CrossX();
    this->Label_Info->setText(this->InfoText);
  }
  
  int NewDialog::getStep()
  {
    return this->step;
  }
}
