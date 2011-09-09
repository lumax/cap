/*
Bastian Ruppert
*/

namespace EuMax01
{
  //  #include "pec_cmd.h"
#ifndef __CAPDIALOGE_H__
#define __CAPDIALOGE_H__

  class ArbeitsDialog;

  class CalibrationDialog : public Screen
  {
  public:
    CalibrationDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
		      ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    void setQ1(unsigned short dat);
    void setQ2(unsigned short dat);
    void setZ1(unsigned short dat);
    void setZ2(unsigned short dat);
    void resetStepValue();
    void incStep();
    void decStep();
    void showStep(int theNewStep);

  private:
    Label * Label_TitleName;
    Label * Label_Step;
    char StepText[256];

    Label * Label_ValueName;
    char ValueName[16];

    Label * Label_Value;
    char Value[64];

    Label * Label_Menue;
    char InfoText[256];

    static const int iQ1 = 0;
    static const int iQ2 = 1;
    static const int iZ1 = 2;
    static const int iZ2 = 3;

    int ActualStep;
    void setXXData(unsigned short dat,int MyStep);
  };

  class MainDialog : public Screen
  {
  public:
    MainDialog(int sdlw,int sdlh,int camw,int camh,int yPos);
  private:
    Button * B1;
    Button * B2;
    Button * B3;
    Button * B4;
    Button * B5;
    Button * B6;
    Button * B7;
    Button * B8;
    Button * Bexit;
  };

  class PositionDialog : public Screen
  {

  public:
    PositionDialog(char* text,short x,short y, unsigned short w,unsigned short h);
    char * getCamCurBuf();
    char * getCamDifBuf();
    char * getCamRefBuf();
    char * getCrossCurBuf();
    char * getCrossRefBuf();

    Label * Label_WertCur;
    Label * Label_WertDif;
    Label * Label_WertRef;

    Label * Label_CrossCur;
    Label * Label_CrossRef;
  private:
    Label * Label_CamPos;
    Label * Label_Cur;
    Label * Label_Dif;
    Label * Label_Ref;

    Label * Label_CrossPos;

    //    Label * Label_Cross_Name;


    char CamCurBuf[64];
    char CamDifBuf[64];
    char CamRefBuf[64];
    char CrossCurBuf[64];
    char CrossRefBuf[64];

  };

  class ErrorDialog;
  class LoadDialog;
  class NewDialog;
  class MBProtocol;
  //class IMBProtListener;

  class IMBProtListener
  {
  public:
    virtual ~IMBProtListener() {}
    virtual void (Q1_evt)(unsigned short dat) = 0;
    virtual void (Q2_evt)(unsigned short dat) = 0;
    virtual void (Z1_evt)(unsigned short dat) = 0;
    virtual void (Z2_evt)(unsigned short dat) = 0;
    virtual void (FP1_evt)(unsigned short dat) = 0;
    virtual void (FP2_evt)(unsigned short dat) = 0;
    virtual void (SWVersion_evt)(unsigned short dat) = 0;
    virtual void (HWVersion_evt)(unsigned short dat) = 0;
  };

  class ArbeitsDialog : public Screen,virtual public IMBProtListener
  {
  public:
    ArbeitsDialog(GUI * pGUI,MBProtocol * prot,\
		  int sdlw,int sdlh, int camw,\
		  int camh,int yPos);
    ~ArbeitsDialog();
    void showRezept(Rezept * pRezept,int nummer);
    void showRezept(int nummer);
    void incRezeptNummer();
    void decRezeptNummer();
    int getRezeptNummer();
    void setCam1Cur(int val);
    void setCam2Cur(int val);
    void setCross1Ref();
    void setCross2Ref();
    void showRecipe(Rezept * rez);

    void Q1_evt(unsigned short dat);
    void Q2_evt(unsigned short dat);
    void Z1_evt(unsigned short dat);
    void Z2_evt(unsigned short dat);
    void FP1_evt(unsigned short dat);
    void FP2_evt(unsigned short dat);
    void SWVersion_evt(unsigned short dat);
    void HWVersion_evt(unsigned short dat);

    GUI * theGUI;
    MBProtocol * theProtocol;
    
    void showLoadDialog(unsigned int page);
    void showArbeitsDialog();
    void showErrorDialog(char * errorMsg);
    void showNewDialog();
    void showCalibrationDialog();

  private:
    Rezept * theRezept;
    int RezeptNummer;
    
    EvtTarget ArbeitsDialogEvtTargets;
    
    SDL_Rect Area;

    int Cam1Cur;
    int Cam1Dif;
    int Cam2Cur;
    int Cam2Dif;

    int Cam1Cur_Cross;
    int Cam1Dif_Cross;
    int Cam2Cur_Cross;
    int Cam2Dif_Cross;

    int iActiveDialog;
    
    static const int ArbeitsDialogIsActive = 0;
    static const int LoadDialogIsActive = 1;
    static const int ErrorDialogIsActive = 2;
    static const int NewDialogIsActive = 3;
    static const int CalDialogIsActive = 4;

    LoadDialog * theLoadDialog;
    ErrorDialog * theErrorDialog;
    NewDialog * theNewDialog;
    CalibrationDialog * theCalDialog;

    //EvtTarget * KeyListener;
    PositionDialog * Pos_Cam1;
    PositionDialog * Pos_Cam2;

    Label * Label_RezeptName;
    Label * Label_RezeptNr1;
    Label * Label_RezeptNr2;
    Label * Label_RezeptNr3;
    Label * Label_RezeptNr4;
    Label * Label_RezeptNr5;
    Label * Label_RezeptNr6;
    Label * Label_RezeptNr7;
    Label * Label_RezeptNr8;
    Label * pLabel_CurrentRezept[8];
    
    Label * Label_InfoF1;
    Label * Label_InfoF2;
    Label * Label_InfoF3;
    Label * Label_InfoF5;
    Label * Label_InfoF6;
    Label * Label_InfoF7;
    Label * Label_InfoF8;
    Label * Label_InfoF12;
    
  };

  class ErrorDialog : public Screen
  {
  public:
    ErrorDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    void setErrorMsg(char * Message);
  private:
    Label * Label_Error;
    Label * Label_Info;
    Label * Label_OK;
    //    Label * Label_Abbruch;
  };

  class LoadDialog : public Screen
  {
  public:
    LoadDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
	       ArbeitsDialog * parent);

    ArbeitsDialog * Parent;
    static const int RezepteLen = 27;
    static const int RezepteProZeile = 9;
    static const int MaxRezeptFileLaenge = 8;
    void setActiveRecipe(int nr);
    void naviUp();
    void naviDown();
    void naviLeft();
    void naviRight();
    void naviReturn();
    void naviPageup();
    void naviPagedown();
    int readSaveDirectory(char * dirName,unsigned int page);
  private:
    int ActiveRecipe;
    int MaxRecipesToDisplay;
    int ActiveSavePage;
    int MaxSavePages;
    void addToActiveRecipe(int summand);
    Label * Label_LadenName;
    Label * pLabel_Rezepte[LoadDialog::RezepteLen];
    char DateiNamen[LoadDialog::RezepteLen][16];
  };


  class NewDialog : public Screen
  {
  public:
    NewDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
	       ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    void incStep();
    void decStep();
    int getStep();
    TextField * TextField_Name;

  private:
    Label * Label_NewName;
    Label * Label_Name;

    Label * Label_Info;
    char InfoText[256];
    Rezept theRecipe;
    int step;
  };
  
#endif /* __CAPDIALOGE_H__*/
}//end namespace
