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
		  int camh,int yPos,char * saveFilePath,bool useGUI);
    ~ArbeitsDialog();

    /*! \brief füllt den einen char Puffer mit data als string
     *
     *  \param tar der u füllende Speicher
     *  \param len Länge des zu füllenden Speichers
     *  \param data Das Datum welches in den String umgewandelt wird
     *  \returns den tar Puffer
     */
    char * int2string(char * tar,int len,int data,char * suffix);
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
    void sendProtocolMsg(char cmd);

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

    int MitteCrossCam1;
    int MitteCrossCam2;

    const char * TextCam1Xaxis;
    const char * TextCam2Xaxis;
    const char * TextZaxis;
    
    void showLoadDialog(unsigned int page,bool loadmode);
    void showArbeitsDialog();
    void showErrorDialog(char * errorMsg);
    void showNewDialog(char * MenuTitel);
    void showCalibrationDialog();


    /*! gibt ein "frisches Rezept zurück
     *
     */
    Rezept * getNullRezept();

    NewDialog * theNewDialog;

    void setColors();

    unsigned int uiC_MenuNormal;
    unsigned int uiC_MenuMarked;
    SDL_Color * pC_MenuText;

    unsigned int uiC_DialogNormal;
    unsigned int uiC_DialogMarked;
    SDL_Color * pC_DialogText;

    unsigned int uiC_WerteNormal;
    unsigned int uiC_WerteMarked;
    SDL_Color * pC_WerteText;
    void blankMenuArea();

  private:
    int CamW_Sichtbar;
    int CamW_Unsichtbar;
    double CamPosConvertStep;

    Rezept * pNullRezept;
    EvtTarget ArbeitsDialogEvtTargets;

    SDL_Rect Area;
    bool useGUI;

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
    static const int NoGUIModeIsActive = 5;

    LoadDialog * theLoadDialog;
    ErrorDialog * theErrorDialog;
    CalibrationDialog * theCalDialog;

    void convertCamPos(int cam,unsigned short dat);

    PosDialog * thePosDialog;
    
    Label * LabelDialogName;
    Label * LabelInfo;
  };

#endif /* __ARBEITSDIALOG_H__*/
}//end namespace
