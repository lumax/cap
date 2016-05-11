/*
Bastian Ruppert
*/

namespace EuMax01
{
  //  #include "pec_cmd.h"
#ifndef __G_TESTDIALOG_H__
#define __G_TESTDIALOG_H__

  class ArbeitsDialog;
  struct t_MenuGLiftSettings;

  class G_TestDialog : public Screen
  {
  public:
    G_TestDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
		      ArbeitsDialog * parent);
    ArbeitsDialog * Parent;
    //void setQ1(unsigned int dat);
    //void setQ2(unsigned int dat);
    //void setZ1(unsigned int dat,char*suffix,unsigned int zFaktor);
    //void setZ2(unsigned int dat,char*suffix);
    //void resetStepValue();
    //void incStep();
    //void decStep();
    //void showStep(int theNewStep);

    static void GLiftUpListener(void*,SDL_Event * evt);
    static void GLiftDownListener(void*,SDL_Event * evt);
    static void GLiftSetSpeedListener(void*,SDL_Event * evt);
    static void escape_listener(void * src, SDL_Event * evt);
    static void left_listener(void * src, SDL_Event * evt);
    static void right_listener(void * src, SDL_Event * evt);
    static void return_listener(void * src, SDL_Event * evt);
    static void GX_LeftListener(void*,SDL_Event * evt);
    static void GX_RightListener(void*,SDL_Event * evt);
    static void GX_SpeedListener(void*,SDL_Event * evt);
    static void GY_LeftListener(void*,SDL_Event * evt);
    static void GY_RightListener(void*,SDL_Event * evt);
    static void GY_SpeedListener(void*,SDL_Event * evt);
    static void GA_UpListener(void*,SDL_Event * evt);
    static void GA_DownListener(void*,SDL_Event * evt);
    static void GA_SpeedListener(void*,SDL_Event * evt);

  private:
    //void setXXData(unsigned int dat,int MyStep,char * suffix);
    //void setXXData(unsigned int dat,int MyStep,char * suffix,unsigned int zFaktor);

    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;

    struct t_MenuGHorizontalSettings theMenuGXSettings;
    MenuGHorizontal * theMenuGX;

    struct t_MenuGHorizontalSettings theMenuGYSettings;
    MenuGHorizontal * theMenuGY;

    struct t_MenuGHorizontalSettings theMenuGWalzeSettings;
    MenuGHorizontal * theMenuGWalze;

    struct t_MenuGHorizontalSettings theMenuGLiftSettings;
    MenuGHorizontal * theMenuGLift;


  };

#endif /* __G_TESTDIALOG_H__*/
}//end namespace
