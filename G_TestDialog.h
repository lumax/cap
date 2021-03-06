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
  class ExaktG;
  class G_Ctrl;
  struct ExaktG_CodeListener_t;

  class G_TestDialog : public Screen
  {
  public:
    G_TestDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
		      ArbeitsDialog * parent);
    ArbeitsDialog * Parent;

    ExaktG * pExaktG;
    G_Ctrl * pGCtrl;
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
    static void return_listener(void * src, SDL_Event * evt);
    static void zeroX_listener(void * src, SDL_Event * evt);
    static void zeroY_listener(void * src, SDL_Event * evt);
    static void zeroA_listener(void * src, SDL_Event * evt);
    static void GX_LeftListener(void*,SDL_Event * evt);
    static void GX_RightListener(void*,SDL_Event * evt);
    static void GX_SpeedListener(void*,SDL_Event * evt);
    static void GY_LeftListener(void*,SDL_Event * evt);
    static void GY_RightListener(void*,SDL_Event * evt);
    static void GY_SpeedListener(void*,SDL_Event * evt);
    static void GA_UpListener(void*,SDL_Event * evt);
    static void GA_DownListener(void*,SDL_Event * evt);
    static void GA_SpeedListener(void*,SDL_Event * evt);

    static void G_MoveBtnMouseOverListener(void* src,bool selected);

    static void getstatus_listener(void* src,SDL_Event * evt);

    void setActive(void);

    void incSpeedLevel(int axis);
    void moveButtonAction(SDL_Event * evt, int Axis,int direction);
  private:
    struct ExaktG_CodeListener_t tGCodeLis;
    static void xPosLis(void * pLis,float pos);
    static void yPosLis(void * pLis,float pos);
    static void zPosLis(void * pLis,float pos);
    static void aPosLis(void * pLis,float pos);
    static void gFLis(void * pLis,int iA,int iB,int iC,int iD);

    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;

    struct t_MenuGSettings theMenuGXSettings;
    MenuGVertical * theMenuGX;

    struct t_MenuGSettings theMenuGYSettings;
    MenuGVertical * theMenuGY;

    struct t_MenuGSettings theMenuGASettings;
    MenuGVertical * theMenuGA;

    struct t_MenuGSettings theMenuGZSettings;
    MenuGVertical * theMenuGZ;

    Button * pBEingabe;
    TextField * pTFEingabe;
    Button * pBZeroX;
    Button * pBZeroY;
    Button * pBZeroA;

  };

#endif /* __G_TESTDIALOG_H__*/
}//end namespace
