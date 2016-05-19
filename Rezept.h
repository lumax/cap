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
  unsigned int x_pos;
  unsigned int z_pos;
  unsigned int x_cross;
  int diameter;
  int walze;
};

struct GPosition_t
{
  float PosGAxisX;
  float PosGAxisY;
  float PosGAxisA;
  float PosGAxisZ;
};

struct PositionSet
{
  struct Position cams[2];
  struct GPosition_t GPosition;
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
    unsigned int getXPosition(int cam,int rezept);
    unsigned int getZPosition(int rezept);
    int getDiameter(int rezept);
    int getWalze(int rezept);
    struct GPosition_t getGPositions(int rezept);
    
    static void copy(Rezept * source,Rezept * target);

    static const int AnzahlRezepte = 8;
    static const int NameLength = 17;
    static const int DefaultDiameter = 200;
    static const int DefaultWalze = 20000;
    char Name[17];
    struct PositionSet Rezepte[8];
    int writeToFile(char * SaveDir);
    int readFromFile(char * FilePath,char * FileName);
  private:
    char buf[1024];
    static const int version = 3000;
  };

#endif /* __REZEPT_H__*/
}//end namespace
