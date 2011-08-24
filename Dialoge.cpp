/*
Bastian Ruppert
*/
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

#include "Dialoge.h"

#include <stdio.h>
namespace EuMax01
{

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
    char zeichen = 0;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_F1)
	  {
	    //ad->theGUI->activateScreen(ad->getLoadDialog());
	    ad->showLoadDialog();
	    printf("F1\n");
	  }
	else if(key->keysym.sym == SDLK_F2)
	  {
	    printf("F2\n");
	  }
	else if(key->keysym.sym == SDLK_F3)
	  {
	    printf("F3\n");
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

  ArbeitsDialog::ArbeitsDialog(GUI * pGUI,	\
			       int sdlw,	\
			       int sdlh,	\
			       int camw,	\
			       int camh,	\
			       int yPos):Screen()
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
    short MIst_y,MSoll_y,MName_y,MInfo_y;

    unsigned short MName_w;
    unsigned short MNameNr_w;
    short MNameSpace_w;
    short MNameNr1_x,MNameNr2_x,MNameNr3_x,MNameNr4_x,MNameNr5_x,MNameNr6_x,MNameNr7_x,MNameNr8_x;

    unsigned short MInfo_w;
    short MInfoSpace_w;
    short MInfoF1_x,MInfoF2_x,MInfoF3_x,MInfoF5_x,MInfoF6_x,MInfoF7_x,MInfoF8_x,MInfoF12_x;

    theRezept = 0;
    RezeptNummer = 0;
    Cam1Cur = 5;
    Cam1Dif = 5;
    Cam2Cur = 6;
    Cam2Dif = 6;

    this->theGUI = pGUI;
    theLoadDialog = new LoadDialog(sdlw,sdlh,camw,camh,yPos,this);

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

    MIst_y  = yPos + 1*MSpace_h + 0*MZeile_h;
    MSoll_y = yPos + 2*MSpace_h + 1*MZeile_h;
    MName_y = yPos + 3*MSpace_h + 2*MZeile_h;
    MInfo_y = yPos + 4*MSpace_h + 3*MZeile_h;

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


    Pos_Cam1 = new PositionDialog("cam1 position",sdlw/2-506,yPos,506,MName_y-yPos);
    Pos_Cam2 = new PositionDialog("cam2 position",sdlw/2,yPos,506,MName_y-yPos);


    /*    Label_IstX1 = new Label("---",MLinks_x,MIst_y,100,MZeile_h);
    Label_IstX1_Name = new Label("current cam:",MLinks_x,MIst_y,200,MZeile_h);
    Label_SollX1 = new Label("---",MLinks_x,MSoll_y,100,MZeile_h);
    Label_SollX1_Name = new Label("reference cam:",MLinks_x,MSoll_y,200,MZeile_h);

    Label_IstX2 = new Label("---",MRechts_x,MIst_y,100,MZeile_h);
    Label_IstX2_Name = new Label("current cam:",MRechts_x,MIst_y,200,MZeile_h);
    Label_SollX2 = new Label("---",MRechts_x,MSoll_y,100,MZeile_h);
    Label_SollX2_Name = new Label("reference cam:",MRechts_x,MSoll_y,200,MZeile_h);
    */

    Label_RezeptName = new Label("----------------",MLinks_x,MName_y,MName_w,MZeile_h);
    Label_RezeptNr1 = new Label("1",MNameNr1_x,MName_y,MNameNr_w,MZeile_h);
    Label_RezeptNr2 = new Label("2",MNameNr2_x,MName_y,MNameNr_w,MZeile_h);
    Label_RezeptNr3 = new Label("3",MNameNr3_x,MName_y,MNameNr_w,MZeile_h);
    Label_RezeptNr4 = new Label("4",MNameNr4_x,MName_y,MNameNr_w,MZeile_h);
    Label_RezeptNr5 = new Label("5",MNameNr5_x,MName_y,MNameNr_w,MZeile_h);
    Label_RezeptNr6 = new Label("6",MNameNr6_x,MName_y,MNameNr_w,MZeile_h);
    Label_RezeptNr7 = new Label("7",MNameNr7_x,MName_y,MNameNr_w,MZeile_h);
    Label_RezeptNr8 = new Label("8",MNameNr8_x,MName_y,MNameNr_w,MZeile_h);

    Label_InfoF1 = new Label("F1: load",MInfoF1_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF2 = new Label("F2: save",MInfoF2_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF3 = new Label("F3: new",MInfoF3_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF5 = new Label("F5: prev step",MInfoF5_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF6 = new Label("F6: next step",MInfoF6_x,MInfo_y,MInfo_w,MZeile_h);
    Label_InfoF7 = new Label("F7:",MInfoF7_x,MInfo_y,MInfo_w,MZeile_h);
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
    
    /*addEvtTarget(Label_IstX1);
    addEvtTarget(Label_IstX1_Name);
    addEvtTarget(Label_SollX1);
    addEvtTarget(Label_SollX1_Name);

    addEvtTarget(Label_IstX2);
    addEvtTarget(Label_IstX2_Name);
    addEvtTarget(Label_SollX2);
    addEvtTarget(Label_SollX2_Name);*/
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

  }

  void ArbeitsDialog::showLoadDialog()
  {
    this->EvtTargets.Next = this->theLoadDialog->EvtTargets.Next;
    this->show(this->theGUI->getMainSurface());
  }

  void ArbeitsDialog::showArbeitsDialog()
  {
    this->EvtTargets.Next = this->ArbeitsDialogEvtTargets.Next;
    this->show(this->theGUI->getMainSurface());
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

  void ArbeitsDialog::showRezept(Rezept * pRezept,int nummer)
  {
    this->theRezept = pRezept;
    this->showRezept(nummer);
  }

  void ArbeitsDialog::setCam1Cur(int val)
  {
    Cam1Cur = val;
    Cam1Dif = Cam1Cur - this->theRezept->getCamPosition(0,RezeptNummer);
    sprintf(this->Pos_Cam1->getCamDifBuf(),"%i",Cam1Dif);
    Pos_Cam1->Label_WertDif->setText(this->Pos_Cam1->getCamDifBuf());
    sprintf(this->Pos_Cam1->getCamCurBuf(),"%i",Cam1Cur);
    Pos_Cam1->Label_WertCur->setText(this->Pos_Cam1->getCamCurBuf());
  }

  void ArbeitsDialog::setCam2Cur(int val)
  {
    Cam2Cur = val;
    Cam2Dif = Cam2Cur - this->theRezept->getCamPosition(1,RezeptNummer);
    sprintf(this->Pos_Cam2->getCamDifBuf(),"%i",Cam2Dif);
    Pos_Cam2->Label_WertDif->setText(this->Pos_Cam2->getCamDifBuf());
    sprintf(this->Pos_Cam2->getCamCurBuf(),"%i",Cam2Cur);
    Pos_Cam2->Label_WertCur->setText(this->Pos_Cam2->getCamCurBuf());
  }

  void ArbeitsDialog::showRezept(int nummer)
  {
    if(theRezept)
      {
	RezeptNummer = nummer;
	sprintf(this->Pos_Cam1->getCamRefBuf(),"%i",theRezept->getCamPosition(0,nummer));
	Pos_Cam1->Label_WertRef->setText(this->Pos_Cam1->getCamRefBuf());
	sprintf(this->Pos_Cam2->getCamRefBuf(),"%i",theRezept->getCamPosition(1,nummer));
	Pos_Cam2->Label_WertRef->setText(this->Pos_Cam2->getCamRefBuf());

	//Difference Berechnen
	this->setCam1Cur(this->Cam1Cur);
	this->setCam2Cur(this->Cam2Cur);

	for(int i=0;i<Rezept::AnzahlRezepte;i++)
	  pLabel_CurrentRezept[i]->setBorder(false);

	pLabel_CurrentRezept[RezeptNummer]->setBorder(true);
      }
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

  void ArbeitsDialog::showRecipe(Rezept * rez)
  {
    theGUI->activateScreen((Screen*)this);
  }

  char * PositionDialog::getCamCurBuf()
  {
    return this->CamCurBuf;
  }
  char * PositionDialog::getCamDifBuf()
  {
    return this->CamDifBuf;
  }
  char * PositionDialog::getCamRefBuf()
  {
    return this->CamRefBuf;
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
    
    short Ax;
    short Ay;
    unsigned short Aw;
    short B1x,B2x,B3x;
    short WertBy,Wert2y;
    unsigned short Bw;
    unsigned short HSpace,VSpace;
    unsigned short LineH;

    CamCurBuf = {'-','-','-'};
    CamDifBuf = {'-','-','-'};
    CamRefBuf = {'-','-','-'};
    CrossCurBuf = {'-','-','-'};
    CrossRefBuf = {'-','-','-'};

    HSpace = 3;
    VSpace = 3;
    Ax  = x+HSpace;
    Ay  = y+VSpace;
    Aw = w/5 - 1*HSpace;
    Bw = (w-(Aw+4*HSpace))/3;
    B1x = x + Aw + 2*HSpace + 0*Bw;
    B2x = x + Aw + 3*HSpace + 1*Bw;
    B3x = x + Aw + 4*HSpace + 2*Bw;

    LineH = (h - 4*VSpace)/3;
    
    WertBy = Ay + LineH + VSpace;
    Wert2y = WertBy + LineH + VSpace;

    Label_CamPos = new Label(text,Ax,Ay,Aw,LineH);
    Label_Cur = new Label("current",B1x,Ay,Bw,LineH);
    Label_Dif = new Label("difference",B2x,Ay,Bw,LineH);
    Label_Ref = new Label("reference",B3x,Ay,Bw,LineH);

    Label_CrossPos = new Label("crossaire pos",Ax,Wert2y,Aw,LineH);
    Label_CrossCur = new Label(CrossCurBuf,B1x,Wert2y,Bw,LineH);
    Label_CrossRef = new Label(CrossRefBuf,B3x,Wert2y,Bw,LineH);

    Label_WertCur = new Label(CamCurBuf,B1x,WertBy,Bw,LineH);
    Label_WertDif = new Label(CamDifBuf,B2x,WertBy,Bw,LineH);
    Label_WertRef = new Label(CamRefBuf,B3x,WertBy,Bw,LineH);
    
    addEvtTarget(Label_CamPos);
    addEvtTarget(Label_Cur);
    addEvtTarget(Label_Dif);
    addEvtTarget(Label_Ref);

    addEvtTarget(Label_CrossPos);
    addEvtTarget(Label_CrossCur);
    addEvtTarget(Label_CrossRef);

    addEvtTarget(Label_WertCur);
    addEvtTarget(Label_WertDif);
    addEvtTarget(Label_WertRef);
  };

  static void LoadDialogKeyListener(void * src, SDL_Event * evt)
  {
    LoadDialog* ad = (LoadDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    char zeichen = 0;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->Parent->showArbeitsDialog();
	  }
	else if(key->keysym.sym == SDLK_F2)
	  {
	    printf("F2\n");
	  }
	else if(key->keysym.sym == SDLK_F3)
	  {
	    printf("F3\n");
	  }
	else if(key->keysym.sym == SDLK_F4)
	  {
	    printf("F4\n");
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
    short MLoadName_y, MLabels_y;

    this->Parent = parent;

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

    Label_LadenName = new Label("LOAD RECIPE",MLinks_x,MLoadName_y,506*2,MZeile_h);
    
    addEvtTarget(Label_LadenName);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->setKeyboardUpEvtHandler(LoadDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!

    //MLabels_y = yPos + 2*MSpace_h + 1*MZeile_h;

  };
}
