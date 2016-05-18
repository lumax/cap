/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __G_POSDIALOG_H__
#define __G_POSDIALOG_H__

  class PosDialog;

  class G_PosDialog : public PosDialog
  {
  public:
    G_PosDialog(char * text,int sdlw,int sdlh, int camw,int camh,int yPos, \
		      ArbeitsDialog * parent);
    ArbeitsDialog * Parent;

    static void G_LeftListener(void*,SDL_Event * evt);
    static void G_RightListener(void*,SDL_Event * evt);
    static void G_SpeedListener(void*,SDL_Event * evt);
    static void G_MoveBtnMouseOverListener(void* src,bool selected);

    static void G_NextAxisButtonListener(void* src,SDL_Event * evt);

    void moveButtonAction(SDL_Event * evt,int axis,int direction);
    void setActive(void);
    void incSpeedLevel(void);
    int getActiveAxis(void);

  private:
    //struct ExaktG_CodeListener_t tGCodeLis;
    static void xPosLis(void * pLis,float pos);
    static void yPosLis(void * pLis,float pos);
    static void zPosLis(void * pLis,float pos);
    static void aPosLis(void * pLis,float pos);
    //static void gFLis(void * pLis,int iA,int iB,int iC,int iD);

    struct t_MenuGSettings theMenuGSettings;
    MenuGVertical * theMenuG;
    int activeAxis;

  };

#endif /* __G_TESTDIALOG_H__*/
}//end namespace
