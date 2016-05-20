/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __G_POSDIALOG_H__
#define __G_POSDIALOG_H__

  class PosDialog;
  class G_GUI;
  struct ExaktG_CodeListener_t;

  class G_PosDialog : public PosDialog
  {
  public:
    G_PosDialog(char * text,int sdlw,int sdlh, int camw,int camh,int yPos, \
		      ArbeitsDialog * parent);
    ArbeitsDialog * Parent;

    static void RezeptAnfahrenListener(void * src, SDL_Event * evt);
    void setActive(void);

    void driveCurrentRecipe(void);
  private:
    Button * pBRezeptAnfahren;
    G_GUI * pG_GUI;
  };

#endif /* __G_TESTDIALOG_H__*/
}//end namespace
