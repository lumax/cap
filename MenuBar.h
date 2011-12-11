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
  struct t_MenuBarSettings;

  class MenuBar
  {
  public:
    MenuBar(int xPos,					\
	    int yPos,					\
	    int height,					\
	    char * Name,				\
	    struct t_MenuBarSettings * settings,	\
	    ArbeitsDialog * Parent);
    ~MenuBar();

    static const int MenuPunkte = 8;
    void addToEvtTarget(EvtTarget* theTarget);

  private:
    Label * LName;
    Button * lbuf[MenuBar::MenuPunkte];
  };

  struct t_MenuBarSettings
  {
    //Dieser Pointer wird bei den Button Events ausgewertet
    //Die Funktionen in evtFnks nutzen diesen Pointer
    void * evtSource;
    char * Text[MenuBar::MenuPunkte];
    void (*evtFnks[MenuBar::MenuPunkte])(void *,SDL_Event * evt);
    
    //function pointer
  };
  
#endif /* __MENUBAR_H__*/
}//end namespace
