/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __REZEPT_H__
#define __REZEPT_H__

  
  /*#ifdef __cplusplus
extern "C" {
#endif*/
struct Position
{
  unsigned short cam;
  unsigned short cross;
};

struct PositionSet
{
  struct Position cams[2];
};
/*#ifdef __cplusplus
}
#endif
*/
  class Rezept
  {
  public:
    Rezept();
    unsigned short getCamPosition(int cam,int rezept);
  private:
    static const int AnzahlRezepte = 8;
    struct PositionSet Rezepte[8];
  };

#endif /* __REZEPT_H__*/
}//end namespace
