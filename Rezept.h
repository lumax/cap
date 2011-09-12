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
  unsigned short z_pos;
  unsigned short cross_x;
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
    unsigned short getCamPosition(int cam,int rezept);
    static const int AnzahlRezepte = 8;
    char Name[16];
    struct PositionSet Rezepte[8];
  private:
  };

#endif /* __REZEPT_H__*/
}//end namespace
