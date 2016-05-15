/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __GMENU_H__
#define __GMENU_H__

  class ArbeitsDialog;

  struct t_MenuGSettings
  {
    //Dieser Pointer wird bei den Button Events ausgewertet
    //Die Funktionen in evtFnks nutzen diesen Pointer
    void * evtSource;
    void (*evtFnkBtn1)(void *,SDL_Event * evt);
    void (*evtFnkBtn2)(void *,SDL_Event * evt);
    void (*evtFnkSetSpeed)(void *,SDL_Event * evt);
    char * btnUpText;
    char * btnDownText;
  };

  class MenuGBase
  {
  public:
    MenuGBase(char * name,				\
	      int xPos,					\
	      int yPos,					\
	      int y_height,				\
	      int y_space,				\
	      int width,				\
	      struct t_MenuGSettings * settings,	\
	      ArbeitsDialog * Parent);
    ~MenuGBase();

    void setMenuName(char * theName);
    void updateSettings(struct t_MenuBarSettings * settings);

    Label * pLName;
    Button * pB1;
    Button * pB2;
    Button * pBSetSpeed;
    Label * pLSpeed;
    Label * pLPosition;

  protected:
    ArbeitsDialog * getArbeitsDialog();
    ArbeitsDialog * theArbeitsDialog;
  };

  class MenuGHorizontal:public MenuGBase
  {
  public:
    MenuGHorizontal(char * name,			\
	      int xPos,					\
	      int yPos,					\
	      int y_height,				\
	      int y_space,				\
	      int width,				\
	      struct t_MenuGSettings * settings,	\
	      ArbeitsDialog * Parent);
    ~MenuGHorizontal();
    void addToEvtTarget(EvtTarget* theTarget);
  };

  class MenuGVertical:public MenuGBase
  {
  public:
    MenuGVertical(char * name,				\
	      int xPos,					\
	      int yPos,					\
	      int y_height,				\
	      int y_space,				\
	      int width,				\
	      struct t_MenuGSettings * settings,	\
	      ArbeitsDialog * Parent);
    ~MenuGVertical();

    void addToEvtTarget(EvtTarget* theTarget);
  };

  
#endif /* __GMENU_H__*/
}//end namespace
