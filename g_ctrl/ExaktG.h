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

    void move(int axis, int direction);

    static char* toString(float f,char* pcTarget,int len);

    float xPos,yPos,zPos,aPos;
    int lastG_F[4];

    static const int AxisX = nTinyG_X;
    static const int AxisY = nTinyG_Y;
    static const int AxisZ = nTinyG_Z;
    static const int AxisA = nTinyG_A;
    static const int MaxAxis = 4;
    static const float G_pro_mm = 0.08375;

    static const float SpeedDistance0in_mm = 1.0;//0.01;
    static const float SpeedDistance1in_mm = 3.0;//0.10;
    static const float SpeedDistance2in_mm = 5.0;//1.00;
    static const float SpeedDistance3in_mm = 10.0;//10.00;

    static const int MaxSpeedLevels = 4;

    //static const char* VelocityDefault = "F100";
    void setSpeedLevel(int axis,int speed);
    void incSpeedLevel(int axis);
    int getSpeedLevel(int axis);

  private:
    PollReader * pr_gcodes;
    PollTimer * pt_gcodes;
    int verbose;
    G_Ctrl GCtrl;
    StreamScanner sScan;
    struct ExaktG_CodeListener_t *ptGCLis;
    void setVelocity(int axis,char * velocity);

    /*
      G1 Y-50.00,F100
          ------      Distance
         -            Axis
	         ---- Velocity

      SpeedLevels = 0,1,2 oder 3
     */
    char * AxisVelocity[ExaktG::MaxAxis];
    int AxisSpeedLevel[ExaktG::MaxAxis];
    float SpeedLevelDistance[ExaktG::MaxSpeedLevels];
  };

#endif /* __ExaktG_H__*/
}//end namespace
