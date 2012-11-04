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
  struct v4l_data_container{
    char * name;
    int v4l_id;
    int cam;
  };

  class InfoDialog : public Screen
  {
  public:
    InfoDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    static const int CamCtrlContLen = 8;
    int aktCamCtrl;
    CamCtrl * CamCtrlContainer[InfoDialog::CamCtrlContLen];

    void setV4L_data(int index);
    void refreshAll();
    void left();
    void right();
    void up();
    void down();
  private:
    struct v4l_data_container v4l_data[16];
    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
  };
 
#endif /* __ERRORDIALOG_H__*/
}//end namespace
