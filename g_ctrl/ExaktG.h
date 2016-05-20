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
    nG_G_f = 5,
    nG_stat = 6
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
    void machineStateChangedEvent(void);
    void setNullpunkt(int axis);

    static char* toString(float f,char* pcTarget,int len);

    //G-Code Status dist: 0 = absolute
    //G-Code Status dist: 1 = incremental
    bool DistanceModeAbsolut;

    //stat machine_state:  1=reset, 2=alarm, 3=stop,
    //4=end, 5=run, 6=hold, 7=probe, 9=homing
    int MachineState;

    static const int DirectionLeft = -1;
    static const int DirectionRight = 1;
    static const int DirectionUp = -1;
    static const int DirectionDown = 1;

    static const int AxisX = nTinyG_X;
    static const int AxisY = nTinyG_Y;
    static const int AxisZ = nTinyG_Z;
    static const int AxisA = nTinyG_A;
    static const int MaxAxis = 4;
    //?tr von 1.25 = G_pro_mm von ca. 0.08375
    //?tr von 1.00 = G_pro_mm von ca. 0.0672333
    //?tr von 1.50 = G_pro_mm von ca. 0.1000
    static const float G_pro_mm = 0.1000;//0.06723333;//0.08333;//0.08375;
    static const float GSicherheitsabstand = 10.00;//10cm
    static const float GMaxStrecke = 89.000;//89cm

/*
  60cm auf der Schiene entsprechen etwa 40,34 G-Werten
  1cm entspricht etwa 0.672333 G-Werten
  Die Schiene hat für einen einzelnen Schlitten einen Verfahrweg von
  etwa 89cm.
  Sicherheitsabstand eines Schlittens zum anderen 10cm
  Maximaler Verfahrweg jedes Schlittens 79cm

  |<---------------------- 89 cm ---------------------------->|
  |-----------------------------------------------------------|
   _X_                                                     _Y_

  Der Nullpunkt für Schlitten X ist auch der Nullpunkt für
  Schlitten Y!!!

*/

    static const float SpeedDistance0in_mm =  0.01;//0.01;
    static const float SpeedDistance1in_mm =  0.10;//0.10;
    static const float SpeedDistance2in_mm =  1.0;//1.00;
    static const float SpeedDistance3in_mm = 10.0;//10.00;
    static const float SpeedDistance4in_mm = 100.0;//10.00;

    static const int MaxSpeedLevels = 5;

    float getXPos(void);
    float getYPos(void);
    float getZPos(void);
    float getAPos(void);
    float getPos(int axis);

    char * getSpeedText(int axis,int SpeedLevel);
    void setSpeedLevel(int axis,int speed);
    void incSpeedLevel(int axis);
    int getSpeedLevel(int axis);
    void move(int axis, int direction);
    void move(int axis, float targetPos,int axisVelocity,bool checkState);
    void holdMoving(bool holdMoving);

    void fastRecipeMove(float tarPosX,float tarPosY,float tarPosA);
    void setCollisionProtektion(bool cp);
    bool getCollisionProtection(void);

  private:
    PollReader * pr_gcodes;
    PollTimer * pt_gcodes;
    int verbose;
    G_Ctrl GCtrl;
    StreamScanner sScan;
    struct ExaktG_CodeListener_t *ptGCLis;
    bool collisionProtection;

    float Position[4];
    int lastG_F[4];

    /*
      G1 Y-50.00,F100
          ------      Distance
         -            Axis
	         ---- Velocity

      SpeedLevels = 0,1,2 oder 3
     */
    int AxisVelocity[ExaktG::MaxAxis];
    int AxisSpeedLevel[ExaktG::MaxAxis];
    float SpeedLevelDistance[ExaktG::MaxSpeedLevels];
    float getAxisDistance(int axis);

    bool isMovingHolded;
    int LastMovedAxis;
    int AxisMoveDirection[ExaktG::MaxAxis];
    void checkForCollision(int axis,float * targetPos);
    float SicherheitsAbstand;
    float MaxXYDistance;
  };

#endif /* __ExaktG_H__*/
}//end namespace
