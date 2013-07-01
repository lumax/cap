/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __FADENKREUZDIALOG_H__
#define __FADENKREUZDIALOG_H__

  class FadenkreuzDialog : public Screen
  {
  public:
    FadenkreuzDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ArbeitsDialog * Parent;

    TextField * TF_Value;
  private:
    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;

    Label * Label_Step;
    char StepText[256];
  };
 
#endif /* __FADENKREUZDIALOG_H__*/
}//end namespace
