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
  unsigned short x_pos;
  unsigned short z_pos;
  unsigned short x_cross;
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
    ~Rezept();
    unsigned short getXPosition(int cam,int rezept);
    static const int AnzahlRezepte = 8;
    char Name[16];
    struct PositionSet Rezepte[8];
  private:
  };

#endif /* __REZEPT_H__*/
}//end namespace
