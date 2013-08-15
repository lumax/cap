/*
Bastian Ruppert
*/

namespace EuMax01
{
#ifndef __ABFRAGEDIALOG_H__
#define __ABFRAGEDIALOG_H__

  class ArbeitsDialog;

  /*  class AbfrageDialog : public Screen
  {
  public:
    AbfrageDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    NewDirectDialog * Parent;
    void setErrorMsg(char * Message);
  private:
    Label * Label_Error;
    Label * Label_Info;
    Label * Label_OK;
    };*/

  class AbfrageDialog : public Screen
  {
  public:
    AbfrageDialog(int sdlw,int sdlh, int camw,int camh,int yPos,	\
		ArbeitsDialog * parent);
    ~AbfrageDialog();
    ArbeitsDialog * Parent;
    void setFileToDelete(char * path,char * filename);
    char * pPathToFile;
  private:
    Label * Label_Info;

    TextField * TF_Value;
    const int TF_Len;
    char Value[32];

    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
  };
 
#endif /* __ABFRAGEDIALOG_H__*/
}//end namespace
