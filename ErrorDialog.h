/*
Bastian Ruppert
*/

namespace EuMax01
{
  //  #include "pec_cmd.h"
#ifndef __ERRORDIALOG_H__
#define __ERRORDIALOG_H__

  class ArbeitsDialog;

  class ErrorDialog : public Screen
  {
  public:
    ErrorDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    void setErrorMsg(char * Message);
  private:
    Label * Label_Error;
    Label * Label_Info;
    Label * Label_OK;
    //    Label * Label_Abbruch;
  };
 
#endif /* __ERRORDIALOG_H__*/
}//end namespace
