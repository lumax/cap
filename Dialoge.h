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
  private:
    Label * Label_A;
    Label * Label_B1;
    Label * Label_B2;
    Label * Label_B3;

    //    Label * Label_Cross_Name;
    Label * Label_WertB1;
    Label * Label_WertB2;
    Label * Label_WertB3;
  };

  class ArbeitsDialog : public Screen
  {
  public:
    ArbeitsDialog(int sdlw,int sdlh, int camw,int camh,int yPos);
    int showRezept(int nummer);
  private:
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
