/*
Bastian Ruppert
*/

namespace EuMax01
{
  //  #include "pec_cmd.h"
#ifndef __MENUBAR_H__
#define __MENUBAR_H__

  class ArbeitsDialog;
  //static const int MenuBar::LabelBufLen;

  struct MenuBarEvtHandler
  {
    char * ButtonName;
    //function pointer
  };

  class MenuBar
  {
  public:
    MenuBar(int xPos,					\
	    int yPos,					\
	    int height,					\
	    char * Name,				\
	    char * LabelTexte[8],	\
	    ArbeitsDialog * Parent);
    ~MenuBar();

    void addToEvtTarget(EvtTarget* theTarget);

  private:
    static const int MenuPunkte = 8;
    Label * LName;
    Label * lbuf[MenuBar::MenuPunkte];
  };
  
#endif /* __MENUBAR_H__*/
}//end namespace
