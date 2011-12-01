/*
Bastian Ruppert
*/

namespace EuMax01
{
  //  #include "pec_cmd.h"
#ifndef __INFODIALOG_H__
#define __INFODIALOG_H__

  class ArbeitsDialog;
  class CamCtrl;

  class InfoDialog : public Screen
  {
  public:
    InfoDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    static const int CamCtrlContLen = 6;
    int aktCamCtrl;
    CamCtrl * CamCtrlContainer[InfoDialog::CamCtrlContLen];
    CamCtrl * Gamma0;
    CamCtrl * Gamma1;
    //    CamCtrl * Bright0;
    //CamCtrl * Bright1;

    void refreshAll();
    void left();
    void right();
  private:
    Label * Label_MenuTitle;
    Label * Label_Menu;
    Label * Label_Info;
    //    Label * Label_Abbruch;
  };
 
#endif /* __ERRORDIALOG_H__*/
}//end namespace
