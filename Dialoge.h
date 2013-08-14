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
    void setQ1(unsigned int dat);
    void setQ2(unsigned int dat);
    void setZ1(unsigned int dat,char*suffix,unsigned int zFaktor);
    void setZ2(unsigned int dat,char*suffix);
    void resetStepValue();
    void incStep();
    void decStep();
    void showStep(int theNewStep);

    static void escape_listener(void * src, SDL_Event * evt);
    static void left_listener(void * src, SDL_Event * evt);
    static void right_listener(void * src, SDL_Event * evt);
    static void return_listener(void * src, SDL_Event * evt);

  private:
    Label * Label_Step;
    char StepText[256];

    Label * Label_ValueName;
    char ValueName[16];

    Label * Label_Value;
    char Value[64];

    int ActualStep;
    void setXXData(unsigned int dat,int MyStep,char * suffix);
    void setXXData(unsigned int dat,int MyStep,char * suffix,unsigned int zFaktor);

    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;

  protected:
    static const int iQ1 = 0;
    static const int iQ2 = 1;
    static const int iZ1 = 2;
    static const int iZ2 = 3;
  };

  class ForceCalDialog : public CalibrationDialog
  {
  public:
    ForceCalDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		   ArbeitsDialog * parent);
    void showForceCalDialog(int problemSource,Screen* pLastActiveScreen);
  private:
    Screen * pLastActiveScreen;
    bool bQ1OverflowPending;
    bool bQ2OverflowPending;
    bool bZ1OverflowPending;
    bool bZ2OverflowPending;
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
    void showDifferenceCam1(int aktuell,int rezept);
    void showDifferenceCam2(int aktuell,int rezept);
    void showDifferenceZ(int aktuell,int rezept);
    
    void showRecipeName(char * text);

  private:
    ArbeitsDialog * Parent;
    static const char DiffZeichenLinks = '>';
    static const char DiffZeichenRechts= '<';
    static const char DiffZeichenMitte = ' ';
    /*! \brief Wieviele Zeichen werden im Extremfall angezeigt
     *
     *  Darf nicht größer oder gleich 64 sein!
     */
    static const unsigned int DiffLen = 16;

    unsigned int DiffFarben[PosDialog::DiffLen];

    /*! \brief füllt den char buffer mit einer Pegelgrafik
     *
     *  \returns GradDerEntfernung, (0-15) je höher, desto weiter weg
     */
    unsigned int printDifference(char target[64],int aktuell,int rezept);
  };

  struct t_LoadDialogButtonEventContainer
  {
    void * theLoadDialog;
    unsigned int index;
  };

  class LoadDialog : public Screen
  {
  public:
    LoadDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
	       ArbeitsDialog * parent);

    ArbeitsDialog * Parent;
    static const unsigned int RezepteLen = 32;
    static const unsigned int RezepteProZeile = 8;
    static const unsigned int MaxRezeptFileLaenge = 15;
    void setActiveRecipe(unsigned int nr);
    void naviUp();
    void naviDown();
    void naviLeft();
    void naviRight();
    void naviReturn();
    void naviPageup();
    void naviPagedown();
    int readSaveDirectory(char * dirName,unsigned int page);
    void setLoadMode(bool loadMode);

    Rezept * tmpRezept;

    static void escape_listener(void * src, SDL_Event * evt);
    static void filter_listener(void * src, SDL_Event * evt);
    static void pgup_listener(void * src, SDL_Event * evt);
    static void pgdown_listener(void * src, SDL_Event * evt);
    static void return_listener(void * src, SDL_Event * evt);
    
    unsigned int getMaxRecipesToDisplay();
    unsigned int getActiveRecipe();

    void clearFilter(void);

  private:
    bool LoadMode;
    unsigned int ActiveRecipe;
    unsigned int MaxRecipesToDisplay;
    int ActiveSavePage;
    int MaxSavePages;
    void addToActiveRecipe(int summand);
    Label * pLabel_Rezepte[LoadDialog::RezepteLen];
    struct t_LoadDialogButtonEventContainer LabelEvtContainers[LoadDialog::RezepteLen];
    char DateiNamen[LoadDialog::RezepteLen][32];
    char * blankText;
    struct t_MenuBarSettings theMenuBarSettings;
    LoadMenuBar * theMenu;
  };

  class NewDirectDialog;
  class WalzeDurchmesser;
  class NewDialog : public Screen
  {
  public:
    NewDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
	       ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    void incStep();
    void decStep();
    int getStep();
    void newDirectReturn(PositionSet * Set);
    void walzeDurchmesserReturn(void);
    void walzeDurchmesserReturn(int durchmesser);
    TextField * TextField_Name;
    Label * LabelWerte[3];
    char pcWerte[3][32];
    static const int iPosX1 = 0;
    static const int iPosX2 = 1;
    static const int iPosZ = 2;
    static const int iPosFP1 = 3;
    static const int iPosZ2 = 4;
    static const int iPosFP2 = 5;
    int sdlw;
    int camw;

    unsigned short usWerte[6];

    void setNewPositionValue(int pos, unsigned int value,char * suffix);
    void setNewPositionValue(int pos, unsigned int value,char * suffix,unsigned int zfaktor);
    void copyValuesToRezept();

    Label * LabelRezept[3];
    char pcRezept[3][64];

    //Label * Label_Menu;

    static const int iMainMenu = 0;
    static const int iCrossaireMenu = 1;
    int theMenuModus;
    void getCam1CrossX();
    void getCam2CrossX();

    /*! brief prüft den Dateinamen auf länge und beschreibt den Parent->theRezept
     *        Namen
     */
    bool verifyName();
    bool keinSpeicherplatz();
    Rezept * tmpRezept;
    
    LL * getDialogsEvtTargets();
    NewDirectDialog * newDirect;
    WalzeDurchmesser * walzeDurchmesser;

    void preparePhaseNewDirect();
    void preparePhaseWalzeDurchmesser();
    void setMenuBarForNewMain();
    void setMenuBarForNewCross();
    MenuBar * theMenu;
    EvtTarget * MouseCrossaire;
  private:
    Label * Label_Name;

    Label * Label_Info;
    char InfoText[64];
    Label * LabelXaxisText;
    Label * LabelZaxisText;
    Label * LabelCrossText;

    void updateRezeptData();
    int step;

    static const int EvtTargetsLen = 15;
    EvtTarget * theEvtTargets[NewDialog::EvtTargetsLen];

    void resetEvtTargets();
    void preparePhaseEnterName();
    void preparePhaseRecipeSteps();
    struct t_MenuBarSettings theMenuBarSettings;
  };

 class OptionsDialog : public Screen
  {
  public:
    OptionsDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ArbeitsDialog * Parent;

  private:
    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
  };
  
#endif /* __CAPDIALOGE_H__*/
}//end namespace
