/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __G_NEWDIALOG_H__
#define __G_NEWDIALOG_H__

  class NewDialog;
  class G_GUI;
  struct ExaktG_CodeListener_t;

  class G_NewDialog : public NewDialog
  {
  public:
    G_NewDialog(int sdlw,int sdlh,	\
			int camw,int camh,int yPos,	\
			ArbeitsDialog * parent);
    ArbeitsDialog * Parent;

    void setActive(void);

  private:
    G_GUI * pG_GUI;
  };

#endif /* __G_NEWDIALOG_H__*/
}//end namespace
