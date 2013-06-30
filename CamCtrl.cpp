/*
Bastian Ruppert
*/
//read directories
#include <stdio.h>

#include <string.h>

#include <errno.h>

#include <SDL/SDL_ttf.h>
#include "LL.h"
#include "Event.h"
#include "Tools.h"
#include "Globals.h"
#include "Button.h"
#include "Poll.h"
#include "Label.h"
#include "Screen.h"
#include "Main.h"

#include <termios.h>
#include <unistd.h>
#include "v4l_capture.h"
#include "Rezept.h"
#include "Protocol.h"
#include "pec_cmd.h"
#include "MBProt.h"

#include "MenuBar.h"
#include "Dialoge.h"
#include "ErrorDialog.h"
#include "ArbeitsDialog.h"

#include <linux/videodev2.h>

#include "V4L2_utils.h"
#include "CamCtrl.h"

namespace EuMax01
{

  CamCtrl::~CamCtrl(){}
  CamCtrl::CamCtrl(int xPos, \
		   int yPos, \
		   int height,\
		   int camNumber, \
		   char * name, \
		   int v4l_id,\
		   ArbeitsDialog * Parent)
  {
    //short M_y;
    //short MLinks_x;
    //unsigned short MSpace_h;
    //unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short x1,x2,x3,x4,x_space, wAll;
    int wName,wButton;

    this->camNumber = camNumber;
    this->v4l_id = v4l_id;
    this->pcName = name;

    for(int i = 0;i < CamCtrl::charBufLen; i++)
      {
	pcMin[i] = '\0';
	pcMax[i] = '\0';
	pcAkt[i] = '\0';
      }
    pcMin[0] = '-';
    pcMax[0] = '-';
    pcAkt[0] = '-';
    pcMin[1] = '-';
    pcMax[1] = '-';
    pcAkt[1] = '-';
    pcMin[2] = '-';
    pcMax[2] = '-';
    pcAkt[2] = '-';   

    x_space = 10;
    wAll = 506;
    wName = 233;
    wButton = 79;

    x1 = xPos;
    x2 = x1 + wName + x_space;
    x3 = x2 + wButton + x_space;
    x4 = x3 + wButton + x_space;
    
    

    LName = new Label(this->pcName,				\
		      x1,yPos,wName,height,Parent->MenuSet);
    
    LMin = new Label(this->pcMin,				\
		     x2,yPos,					\
		     wButton,height);  

    LAkt = new TextField("---",CamCtrl::charBufLen,			\
			 x3,						\
			 yPos,wButton,			\
			 height,Parent->WerteSet);
    LAkt->activateKeyListener(TextField::IntegerNumericChar);
    LAkt->setBorder(false);
    //LAkt->hide(true);
    LAkt->setActive(false);

    LMax = new Label(this->pcMax,				\
		     x4,yPos,					\
		     wButton,height);  

    /*    TF_Value = new TextField("",TF_Len,				\
			     x2,					\
			     Zeile3_y,button_schmal,			\
			     MZeile_h,Parent->Parent->WerteSet);
    TF_Value->activateKeyListener(TextField::IntegerNumericChar);
    TF_Value->setBorder(false);
    TF_Value->hide(true);
    TF_Value->setActive(true);
    */
    //this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    //this->bHide = true;
    //this->setKeyboardUpEvtHandler(NewDirectKeyListener);
    //this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    //addEvtTarget(Label_Menu);
    setFocus(true);
  }

  void CamCtrl::setV4L_Data(int id,char * name, int cam)
  {
    this->v4l_id = id;
    this->pcName = name;
    this->LName->setText(this->pcName);
    this->camNumber = cam;
  }

  void CamCtrl::addToEvtTarget(EvtTarget* theTarget)
  {
    theTarget->addEvtTarget(LName);
    theTarget->addEvtTarget(LMin);
    theTarget->addEvtTarget(LAkt);
    theTarget->addEvtTarget(LMax);
  }

  void CamCtrl::refreshValues()
  {
    struct v4l2_queryctrl queryctrl;
    if(!getV4L2QueryCtrl(cap_cam_getFd(this->camNumber),this->v4l_id,&queryctrl))
      {
	/*
	  printf("\tminmum : %i\n",queryctrl.minimum);
	  printf("\tmaximum : %i\n",queryctrl.maximum);
	  printf("\tstep : %i\n",queryctrl.step);
	*/
	this->step = queryctrl.step;
	this->min = queryctrl.minimum;
	this->max = queryctrl.maximum;
	snprintf(this->pcMin,CamCtrl::charBufLen,"%i",this->min);
	this->LMin->setText(this->pcMin);
	snprintf(this->pcMax,CamCtrl::charBufLen,"%i",this->max);
	this->LMax->setText(this->pcMax);
      }
    else
      {
	this->LMin->setText("error");
	this->LMax->setText("error");
      }
    if(!getV4L2_Value(cap_cam_getFd(this->camNumber),this->v4l_id,&this->value))
      {
	snprintf(this->pcAkt,CamCtrl::charBufLen,"%i",this->value);
	this->LAkt->setText(this->pcAkt);
      }
    else
      {
	this->LAkt->setText((char *)"");
      }
  }

  void CamCtrl::setFocus(bool focus)
  {
    if(focus)
      {
	this->refreshValues();
	this->LAkt->setBorder(true);
	this->LAkt->setActive(true);
      }
    else
      {
	this->LAkt->setBorder(false);
	this->LAkt->setActive(false);
      }
  }

  void CamCtrl::setValue(int wert)
  {
    setV4L2_Value(cap_cam_getFd(this->camNumber),this->v4l_id,wert);
    if(getV4L2_Value(cap_cam_getFd(this->camNumber),this->v4l_id,&this->value))
      {
	this->LAkt->setText((char *)"error");
      }
    else
      {
	snprintf(this->pcAkt,CamCtrl::charBufLen,"%i",this->value);
	this->LAkt->setText(this->pcAkt);	
      }
  }

  void CamCtrl::up()
  {
    setValue(this->value + this->step);
  }

  void CamCtrl::down()
  {
    setValue(this->value - this->step);    
  }

  void CamCtrl::keyEventOccured(SDL_KeyboardEvent * key)
  {
    int tf_value;
    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_RETURN)
	  {
	    tf_value = atof(LAkt->getText());
	    if(tf_value<this->min)
	      {
		tf_value = this->min;
		snprintf(this->pcAkt,CamCtrl::charBufLen,"%i",tf_value);
		this->LAkt->setText(this->pcAkt);
	      }
	    else if(tf_value > this->max)
	      {
		tf_value = this->max;
		snprintf(this->pcAkt,CamCtrl::charBufLen,"%i",tf_value);
		this->LAkt->setText(this->pcAkt);
	      }
	    setValue(tf_value);
	  }
      }
  }
}
