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
    void setMenuName(char * theName);
    void updateSettings(struct t_MenuBarSettings * settings);

  protected:
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

  class LoadMenuBar : public MenuBar
  {
  public:
    LoadMenuBar(int xPos,					\
	    int yPos,					\
	    int height,					\
	    char * Name,				\
	    struct t_MenuBarSettings * settings,	\
		ArbeitsDialog * Parent,\
		int MaxFilterTextLen);
    ~LoadMenuBar();

    TextField * TextField_Filter;

    void addToEvtTarget(EvtTarget* theTarget);
  };
  
#endif /* __MENUBAR_H__*/
}//end namespace
