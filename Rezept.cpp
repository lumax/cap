/*
Bastian Ruppert
*/
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

#include "Rezept.h"

namespace EuMax01
{

  /*static void evtB1(void * src,SDL_Event * evt){
  cap_cam_addCrossX(0,-10);
}

static void evtExit(void * src,SDL_Event * evt){
  GUI::getInstance(0,0)->stopEventLoop();
  }*/

  Rezept::Rezept()
  {
    int i;
    for(i=0;i<Rezept::AnzahlRezepte;i++)
      {
	this->Rezepte[i].cams[0].x_pos = 0;
	this->Rezepte[i].cams[0].z_pos = 0;
	this->Rezepte[i].cams[0].x_cross = 0;

	this->Rezepte[i].cams[1].x_pos = 0;
	this->Rezepte[i].cams[1].z_pos = 0;
	this->Rezepte[i].cams[1].x_cross = 0;
      }

    for(i=0;i<16;i++)
      {
	this->Name[i] = '\0';
      }
  };

  Rezept::~Rezept()
  {

  }

  unsigned short Rezept::getXPosition(int cam,int rezept)
  {
    int theCam = 0;
    int pos;

    if(cam)
      theCam = 1;

    if(rezept<0)
      pos = 0;
    else if(rezept>=Rezept::AnzahlRezepte)
      pos = Rezept::AnzahlRezepte-1;
    else
      pos = rezept;
    
    return this->Rezepte[pos].cams[theCam].x_pos;
  }

}
