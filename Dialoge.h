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

  class PosDialog : public Screen
  {

  public:
    PosDialog(char* text,int sdlw,int sdlh, int camw,int camh,int yPos,\
	      ArbeitsDialog * parent);
    ~PosDialog();
 
    Label * LabelRecipeName;
    Label * LabelStep;
    Label * LabelActual;

    static const int iStep = 0;
    static const int iDiff = 1;
    static const int iCurr = 2;
    static const int iLabelName = 3;

    Label * pLabelCam1[4];
    Label * pLabelCam2[4];
    Label * pLabelZ[4];
    char pcLabelCam1[3][64];
    char pcLabelCam2[3][64];
    char pcLabelZ[3][64];
    char pcLabelRecipeName[64];

    void setRecipe(Rezept * pRec);
    void setStep(unsigned int step);
    void showDifferenceCam1(int diff);
    void showDifferenceCam2(int diff);
    void showDifferenceZ(int diff);
    
    void showRecipeName(char * text);

  private:
    
    ArbeitsDialog * Parent;
    static const char DiffLeerzeichen = ' ';
    static const char DiffLen = 62;
    static const char DiffLine = '|';
    /*! \brief füllt den char buffer mit einer Pegelgrafik
     *
     */
    void printDifference(char target[64],int diff);
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
		  int camh,int yPos,char * saveFilePath);
    ~ArbeitsDialog();

    /*! \brief füllt den einen char Puffer mit data als string
     *
     *  \param tar der u füllende Speicher
     *  \param len Länge des zu füllenden Speichers
     *  \param data Das Datum welches in den String umgewandelt wird
     *  \returns den tar Puffer
     */
    char * int2string(char * tar,int len,int data);
    //    void showRezept(Rezept * pRezept,int nummer);
    void showRezept(int nummer);
    void incRezeptNummer();
    void decRezeptNummer();
    int getRezeptNummer();
    void setCam1XaxisCur(int val);
    void setCam2XaxisCur(int val);
    void setCam1ZaxisCur(int val);
    void setCam2ZaxisCur(int val);
    void setCross1Ref();
    void setCross2Ref();
    //void showRecipe(Rezept * rez);

    void Q1_evt(unsigned short dat);
    void Q2_evt(unsigned short dat);
    void Z1_evt(unsigned short dat);
    void Z2_evt(unsigned short dat);
    void FP1_evt(unsigned short dat);
    void FP2_evt(unsigned short dat);
    void SWVersion_evt(unsigned short dat);
    void HWVersion_evt(unsigned short dat);

    int RezeptNummer;
    Rezept * theRezept;
    GUI * theGUI;
    MBProtocol * theProtocol;
    char * pcSaveFilePath;
    
    void showLoadDialog(unsigned int page);
    void showArbeitsDialog();
    void showErrorDialog(char * errorMsg);
    void showNewDialog();
    void showCalibrationDialog();

  private:
    EvtTarget ArbeitsDialogEvtTargets;

    SDL_Rect Area;

    int Cam1XaxisCur;
    int Cam1XaxisDif;
    int Cam2XaxisCur;
    int Cam2XaxisDif;

    int Cam1ZaxisCur;
    int Cam1ZaxisDif;
    int Cam2ZaxisCur;
    int Cam2ZaxisDif;

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
    PosDialog * thePosDialog;
    
    Label * LabelDialogName;
    Label * LabelInfo;
    
    /*    Label * Label_InfoF1;
    Label * Label_InfoF2;
    Label * Label_InfoF3;
    Label * Label_InfoF5;
    Label * Label_InfoF6;
    Label * Label_InfoF7;
    Label * Label_InfoF8;
    Label * Label_InfoF12;*/
    
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
    static const unsigned int RezepteLen = 27;
    static const unsigned int RezepteProZeile = 9;
    static const unsigned int MaxRezeptFileLaenge = 8;
    void setActiveRecipe(unsigned int nr);
    void naviUp();
    void naviDown();
    void naviLeft();
    void naviRight();
    void naviReturn();
    void naviPageup();
    void naviPagedown();
    int readSaveDirectory(char * dirName,unsigned int page);
  private:
    Rezept * tmpRezept;
    int ActiveRecipe;
    unsigned int MaxRecipesToDisplay;
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
    Label * LabelWerte[6];
    char pcWerte[6][32];
    static const int iPosQ1 = 0;
    static const int iPosZ1 = 1;
    static const int iPosFP1 = 2;

    static const int iPosQ2 = 3;
    static const int iPosZ2 = 4;
    static const int iPosFP2 = 5;
    unsigned short usWerte[6];

    void setNewPositionValue(int pos, unsigned short value);
    void copyValuesToRezept();

    Label * LabelRezept[6];
    char pcRezept[6][64];

    Label * Label_Menu;

    static const int iMainMenu = 0;
    static const int iCrossaireMenu = 1;
    int theMenuModus;
    void getCam1CrossX();
    void getCam2CrossX();
  private:
    Rezept * pRezept;
    //Label * Label_NewName;
    Label * Label_Name;

    Label * Label_Info;
    char InfoText[64];
    Label * LabelXaxisText;
    Label * LabelZaxisText;
    Label * LabelCrossText;

    void updateRezeptData();

    Rezept theRecipe;
    int step;
  };
  
#endif /* __CAPDIALOGE_H__*/
}//end namespace
