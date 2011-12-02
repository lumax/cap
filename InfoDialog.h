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
    static const int CamCtrlContLen = 8;
    int aktCamCtrl;
    CamCtrl * CamCtrlContainer[InfoDialog::CamCtrlContLen];

    void refreshAll();
    void left();
    void right();
    void up();
    void down();
  private:
    Label * Label_MenuTitle;
    Label * Label_Menu;
    //Label * Label_Info;
  };
 
#endif /* __ERRORDIALOG_H__*/
}//end namespace
