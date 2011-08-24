/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __CAPDIALOGE_H__
#define __CAPDIALOGE_H__

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
  
  class LoadDialog;

  class ArbeitsDialog : public Screen
  {
  public:
    ArbeitsDialog(GUI * pGUI,int sdlw,int sdlh, int camw,int camh,int yPos);
    void showRezept(Rezept * pRezept,int nummer);
    void showRezept(int nummer);
    void incRezeptNummer();
    void decRezeptNummer();
    int getRezeptNummer();
    void setCam1Cur(int val);
    void setCam2Cur(int val);
    void setCross1Ref();
    void setCross2Ref();
    Screen * getLoadDialog();
    void showRecipe(Rezept * rez);

    GUI * theGUI;

  private:
    Rezept * theRezept;
    int RezeptNummer;
    
    int Cam1Cur;
    int Cam1Dif;
    int Cam2Cur;
    int Cam2Dif;

    LoadDialog * theLoadDialog;

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
  
  class LoadDialog : public Screen
  {
  public:
    LoadDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
	       ArbeitsDialog * parent);

    ArbeitsDialog * Parent; 

  private:
    static const int RezepteLen = 64;
    Label * Label_LadenName;
    Label * pLabel_Rezepte[LoadDialog::RezepteLen];
  };
  
#endif /* __CAPDIALOGE_H__*/
}//end namespace
