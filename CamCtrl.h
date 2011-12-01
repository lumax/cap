/*
Bastian Ruppert
*/

namespace EuMax01
{
  //  #include "pec_cmd.h"
#ifndef __CAMCTRL_H__
#define __CAMCTRL_H__

  class ArbeitsDialog;

  class CamCtrl
  {
  public:
    CamCtrl(int xPos, \
	    int yPos,				\
	    int height,				\
	    int camNumber,				\
	    char * name,			\
	    int v4l_id,\
	    ArbeitsDialog * Parent);
    ~CamCtrl();

    void addToEvtTarget(EvtTarget* theTarget);
    void setFocus(bool focus);
    void keyEventOccured(SDL_KeyboardEvent * key);

    void up();
    void down();
    //void up_f();
    //void down_f();
  private:
    static const int charBufLen = 64;
    int camNumber;
    int v4l_id;

    int value;
    int min;
    int max;
    int step;

    char * pcName;
    Label * LName;
    Label * LMin;
    Label * LMax;
    TextField * LAkt;
    char pcMin[CamCtrl::charBufLen];
    char pcMax[CamCtrl::charBufLen];
    char pcAkt[CamCtrl::charBufLen];
    void setValue(int wert);
    void refreshValues();
  };
  
#endif /* __CAMCTRL_H__*/
}//end namespace
