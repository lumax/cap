/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __CAPDIALOGE_H__
#define __CAPDIALOGE_H__

  class MainDialog : public Screen
  {
  public:
    MainDialog(int sdlw,int sdlh,int camw,int camh,int yPos);
  private:
    Button * B1;
    Button * B2;
    Button * B3;
    Button * B4;
    Button * B5;
    Button * B6;
    Button * B7;
    Button * B8;
    Button * Bexit;
  };


#endif /* __CAPDIALOGE_H__*/
}//end namespace
