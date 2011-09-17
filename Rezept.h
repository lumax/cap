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
    unsigned short getZPosition(int rezept);
    
    static void copy(Rezept * source,Rezept * target);

    static const int AnzahlRezepte = 8;
    static const int NameLength = 17;
    char Name[17];
    struct PositionSet Rezepte[8];
    int writeToFile(char * SaveDir);
    int readFromFile(char * FilePath,char * FileName);
  private:
    char buf[1024];
    static const int version = 1000;
  };

#endif /* __REZEPT_H__*/
}//end namespace
