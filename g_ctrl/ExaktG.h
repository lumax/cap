/*
Bastian Ruppert
*/

namespace EuMax01
{
#ifndef __ExaktG_H__
#define __ExaktG_H__

  enum TinyG_Answers{
    nG_posX = 1,
    nG_posY = 2,
    nG_posZ = 3,
    nG_posA = 4,
    nG_G_f = 5
  };

  struct ExaktG_CodeListener_t{
    void * pTheListener;
    void (*fnkXPosLis)(void * pLis,float xPos);
    void (*fnkYPosLis)(void * pLis,float yPos);
    void (*fnkZPosLis)(void * pLis,float zPos);
    void (*fnkAPosLis)(void * pLis,float aPos);
    void (*fnkGFLis)(void*,int iA,int iB, int iC, int iD);
  };

  class ExaktG:IPollReadListener,IPollTimerListener
  {
  public:
    ExaktG(int verbExakt,int verbGCtrl);
    ~ExaktG(){
      delete(pr_gcodes);
    }

    void setVerbose(int verb);
    virtual void (pollReadEvent)(PollSource * s);
    virtual void (pollTimerExpired)(long time);
    void setFD(int fd);
    PollReader* getPollReader(void);
    PollTimer* getPollTimer(void);
    G_Ctrl * getG_Ctrl(void);
    void setCoordinatesRelative(void);
    void setCoordinatesAbsolute(void);
    static void streamScanResult(struct StreamScanner_t * ps);
    void setGCodeResultListener(struct ExaktG_CodeListener_t* pLis);
    //struct ExaktG_CodeListener_t* getGCodeResultListener(void);

    static char* toString(float f,char* pcTarget,int len);

    float xPos,yPos,zPos,aPos;
    int lastG_F[4];

  private:
    PollReader * pr_gcodes;
    PollTimer * pt_gcodes;
    int verbose;
    G_Ctrl GCtrl;
    StreamScanner sScan;
    struct ExaktG_CodeListener_t *ptGCLis;
  };

#endif /* __ExaktG_H__*/
}//end namespace
