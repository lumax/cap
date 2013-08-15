/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __ARBEITSDIALOG_H__
#define __ARBEITSDIALOG_H__

  class CalibrationDialog;
  class PosDialog;
  class ErrorDialog;
  class ConfirmDialog;
  class SplashScreen;
  class LoadDialog;
  class NewDialog;
  class MBProtocol;
  class InfoDialog;
  class OptionsDialog;
  class FadenkreuzDialog;
  class AbfrageDialog;
  //class IMBProtListener;
  class MenuBar;

  class IMBProtListener
  {
  public:
    virtual ~IMBProtListener() {}
    virtual void (Q1_evt)(unsigned int dat) = 0;
    virtual void (Q2_evt)(unsigned int dat) = 0;
    virtual void (Z1_evt)(unsigned int dat) = 0;
    virtual void (Z2_evt)(unsigned int dat) = 0;
    virtual void (FP1_evt)(unsigned int dat) = 0;
    virtual void (FP2_evt)(unsigned int dat) = 0;
    virtual void (SWVersion_evt)(unsigned short dat) = 0;
    virtual void (HWVersion_evt)(unsigned short dat) = 0;
  };

  class ArbeitsDialog : public Screen,virtual public IMBProtListener
  {
  public:
    ArbeitsDialog(GUI * pGUI,MBProtocol * prot,	\
		  int sdlw,int sdlh, int camw,				\
		  int camh,int yPos,char * saveFilePath,bool useGUI,	\
		  void (*newDiameterFnk)(int),				\
		  int (*getDiameterFnk)(void),\
		  void (*setFKBreite)(int val),		\
		  void (*saveFKBreite)(void),\
		  int (*getFKBreite)(void));
    ~ArbeitsDialog();

    /*! \brief füllt den einen char Puffer mit data als string
     *
     *  \param tar der u füllende Speicher
     *  \param len Länge des zu füllenden Speichers
     *  \param data Das Datum welches in den String umgewandelt wird
     *  \returns den tar Puffer
     */
    char * int2string(char * tar,int len,int data,char * suffix);

    char * int2string(char * tar,int len,int data,char * suffix,unsigned int zFaktor);
    //    void showRezept(Rezept * pRezept,int nummer);
    void showRezept(int nummer);
    void incRezeptNummer();
    void decRezeptNummer();
    int getRezeptNummer();
    void setCam1XaxisCur(int val);
    void setCam2XaxisCur(int val);
    void setCam1ZaxisCur(int val,char*suffix,unsigned int FaktorZAchse);
    void setCam2ZaxisCur(int val,char*suffix,unsigned int FaktorZAchse);
    void setCross1Ref();
    void setCross2Ref();
    //void showRecipe(Rezept * rez);
    void sendProtocolMsg(char cmd);
    void sendProtocolMsg(char cmd,int val);

    void setFaktorZAchse(int i);
    int getFaktorZAchse(void);

    void Q1_evt(unsigned int dat);
    void Q2_evt(unsigned int dat);
    void Z1_evt(unsigned int dat);
    void Z2_evt(unsigned int dat);
    void FP1_evt(unsigned int dat);
    void FP2_evt(unsigned int dat);
    void SWVersion_evt(unsigned short dat);
    void HWVersion_evt(unsigned short dat);

    float convertMBProtData(unsigned int dat,float faktor);
    float convertMBProtData(unsigned int dat);

    int RezeptNummer;
    Rezept * theRezept;
    GUI * theGUI;
    MBProtocol * theProtocol;
    char * pcSaveFilePath;

    int MitteCrossCam1;
    int MitteCrossCam2;

    const char * TextCam1Xaxis;
    const char * TextCam2Xaxis;
    const char * TextZaxis;
    
    void showLoadDialog(unsigned int page,bool loadmode);
    void showArbeitsDialog();
    void showErrorDialog(char * errorMsg);
    void showConfirmDialog(char * msg);
    void showSplashScreen();
    void showNewDialog(char * MenuTitel);
    void showCalibrationDialog();
    void showInfoDialog();
    void showOptionsDialog();
    void showFadenkreuzDialog();
    void showFileDeleteAbfrageDialog(char* delPath,char* fileName);

    static const int ArbeitsDialogIsActive = 0;
    static const int LoadDialogIsActive = 1;
    static const int ErrorDialogIsActive = 2;
    static const int NewDialogIsActive = 3;
    static const int CalDialogIsActive = 4;
    static const int NoGUIModeIsActive = 5;
    static const int ConfirmDialogIsActive = 6;
    static const int InfoDialogIsActive = 7;
    static const int SplashScreenIsActive = 8;
    static const int OptionsDialogIsActive = 9;
    static const int FadenkreuzDialogIsActive = 10;
    static const int FileDeleteAbfrageDialogIsActive = 11;

    int theActiveDialogNumber();

    void (*newCircleDiameterFnk)(int);
    int (*getCircleDiameterFnk)(void);

    void (*setCrossairWidth)(int val);
    void (*saveCrossairWidth)(void);
    int (*getCrossairWidth)(void);

    /*! gibt ein "frisches Rezept zurück
     *
     */
    Rezept * getNullRezept();

    NewDialog * theNewDialog;

    ButtonSettings * MenuSet;
    ButtonSettings * DialogSet;
    ButtonSettings * WerteSet;

    void blankMenuArea();
    void blankButton(Button * b);

    unsigned short getCamW_Sichtbar();
    bool useTheGUI();
    void convertCamPos(int cam,unsigned short dat);
    void clearFilter(void);

  private:
    int CamW_Sichtbar;
    int CamW_Unsichtbar;

    Rezept * pNullRezept;
    EvtTarget ArbeitsDialogEvtTargets;

    SDL_Rect Area;
    bool useGUI;

    int iFaktorZAchse;

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

    LoadDialog * theLoadDialog;
    ErrorDialog * theErrorDialog;
    ConfirmDialog * theConfirmDialog;
    SplashScreen * theSplashScreen;
    CalibrationDialog * theCalDialog;
    InfoDialog * theInfoDialog;
    OptionsDialog * theOptionsDialog;
    FadenkreuzDialog * theFadenkreuzDialog;
    AbfrageDialog * theFileDeleteAbfrageDialog;

    PosDialog * thePosDialog;
    
    Label * LabelDialogName;
    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
  };

#endif /* __ARBEITSDIALOG_H__*/
}//end namespace
