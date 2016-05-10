/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __GMENU_H__
#define __GMENU_H__

  class ArbeitsDialog;

  struct t_MenuGLiftSettings
  {
    //Dieser Pointer wird bei den Button Events ausgewertet
    //Die Funktionen in evtFnks nutzen diesen Pointer
    void * evtSource;
    void (*evtFnkUp)(void *,SDL_Event * evt);
    void (*evtFnkDown)(void *,SDL_Event * evt);
    void (*evtFnkSetSpeed)(void *,SDL_Event * evt);
  };

  class MenuGLift
  {
  public:
    MenuGLift(int xPos,					\
	      int yPos,					\
	      int y_height,				\
	      int y_space,				\
	      int width,				\
	      struct t_MenuGLiftSettings * settings,	\
	      ArbeitsDialog * Parent);
    ~MenuGLift();

    static const int MenuPunkte = 8;
    void addToEvtTarget(EvtTarget* theTarget);
    void setMenuName(char * theName);
    void updateSettings(struct t_MenuBarSettings * settings);

  protected:
    Label * pLName;
    Label * pLSpeed;
    Button * pBUp;
    Button * pBDown;
    Button * pBSetSpeed;
  };
  
#endif /* __GMENU_H__*/
}//end namespace
