/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __G_CALIBRATIONDIALOG_H__
#define __G_CALIBRATIONDIALOG_H__

  class CalibrationDialog;
  class G_GUI;
  struct ExaktG_CodeListener_t;

  class G_CalibrationDialog : public CalibrationDialog
  {
  public:
    G_CalibrationDialog(int sdlw,int sdlh,	\
			int camw,int camh,int yPos,	\
			ArbeitsDialog * parent);
    ArbeitsDialog * Parent;

    void setActive(void);

  private:
    G_GUI * pG_GUI;
  };

#endif /* __G_CALIBRATIONDIALOG_H__*/
}//end namespace
