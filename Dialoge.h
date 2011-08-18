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
    char * getCrossRefBuf();

    Label * Label_WertCur;
    Label * Label_WertDif;
    Label * Label_WertRef;
  private:
    Label * Label_A;
    Label * Label_Cur;
    Label * Label_Dif;
    Label * Label_Ref;

    //    Label * Label_Cross_Name;


    char CamCurBuf[16];
    char CamDifBuf[16];
    char CamRefBuf[16];
    char CrossRefBuf[16];

  };

  class ArbeitsDialog : public Screen
  {
  public:
    ArbeitsDialog(int sdlw,int sdlh, int camw,int camh,int yPos);
    void showRezept(Rezept * pRezept,int nummer);
    void showRezept(int nummer);
    void incRezeptNummer();
    void decRezeptNummer();
    int getRezeptNummer();
    void setCam1Cur(int val);
    void setCam2Cur(int val);

  private:
    Rezept * theRezept;
    int RezeptNummer;
    
    int Cam1Cur;
    int Cam1Dif;
    int Cam2Cur;
    int Cam2Dif;

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

  
#endif /* __CAPDIALOGE_H__*/
}//end namespace
