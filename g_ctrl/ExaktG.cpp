/*
Bastian Ruppert
*/

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "LL.h"
#include "Poll.h"
#include "G_Ctrl.h"
#include "StreamScanner.h"

#include "ExaktG.h"

namespace EuMax01
{

  char* ExaktG::toString(float f,char * pcTarget, int len)
  {
    for(int i=0;i<len;i++){
      pcTarget[i]='\0';
    }
    snprintf(pcTarget,len,"%.0003f",f);
    return pcTarget;
  }

  void ExaktG::streamScanResult(struct StreamScanner_t * ps)
  {
    ExaktG * pEG = (ExaktG *)ps->userPnt;

    if(0==pEG){
      return;
    }

    if(0==pEG->ptGCLis){
	return;
    }

    switch(ps->userID){
    case nG_posX:{
      pEG->Position[ExaktG::AxisX] = ps->scannedFloat;
      if(0!=pEG->ptGCLis->fnkXPosLis){
	(pEG->ptGCLis->fnkXPosLis)(pEG->ptGCLis->pTheListener,	\
				   ps->scannedFloat);
      }
      break;
    }
    case nG_posY:{
      pEG->Position[ExaktG::AxisY] = ps->scannedFloat;
      if(0!=pEG->ptGCLis->fnkYPosLis){
	(pEG->ptGCLis->fnkYPosLis)(pEG->ptGCLis->pTheListener,	\
				   ps->scannedFloat);
      }
      break;
    }
    case nG_posZ:{
      pEG->Position[ExaktG::AxisZ] = ps->scannedFloat;
      if(0!=pEG->ptGCLis->fnkZPosLis){
	(pEG->ptGCLis->fnkZPosLis)(pEG->ptGCLis->pTheListener,	\
				   ps->scannedFloat);
      }
      break;
    }
    case nG_posA:{
      pEG->Position[ExaktG::AxisA] = ps->scannedFloat;
      if(0!=pEG->ptGCLis->fnkAPosLis){
	(pEG->ptGCLis->fnkAPosLis)(pEG->ptGCLis->pTheListener,	\
				   ps->scannedFloat);
      }
      break;
    }
    case nG_G_f:{
      pEG->lastG_F[0] = ps->scannedG_F[0];
      pEG->lastG_F[1] = ps->scannedG_F[1];
      pEG->lastG_F[2] = ps->scannedG_F[2];
      pEG->lastG_F[3] = ps->scannedG_F[3];
      if(0!=pEG->ptGCLis->fnkGFLis){
	(pEG->ptGCLis->fnkGFLis)(pEG->ptGCLis->pTheListener,	\
				 ps->scannedG_F[0],		\
				 ps->scannedG_F[1],	\
				 ps->scannedG_F[2],	\
				 ps->scannedG_F[3]);
      }
      break;
    case nG_stat:{
      pEG->MachineState = ps->scannedInt;
      pEG->machineStateChangedEvent();
      break;
    }
    }
    default:{
      if(0!=pEG->verbose){
	printf("ExaktG::stremScanResult: unknown userID\n");
      }
      break;
    }
    }

  }

  ExaktG::ExaktG(int verbExakt,int verbG):GCtrl(verbG)
  {
    this->setVerbose(verbExakt);
    pr_gcodes = new PollReader(this);
    pt_gcodes = new PollTimer(-1,this);

    lastG_F[0] = 0;
    lastG_F[1] = 0;
    lastG_F[2] = 0;
    lastG_F[3] = 0;

    AxisMoveDirection[ExaktG::AxisX] = ExaktG::DirectionLeft;
    AxisMoveDirection[ExaktG::AxisY] = ExaktG::DirectionRight;
    AxisMoveDirection[ExaktG::AxisZ] = ExaktG::DirectionUp;
    AxisMoveDirection[ExaktG::AxisA] = ExaktG::DirectionUp;
    LastMovedAxis = ExaktG::AxisX;
    isMovingHolded = false;

    collisionProtection = true;
    DistanceModeAbsolut = true;
    MachineState = 0;
    SicherheitsAbstand = ExaktG::GSicherheitsabstand;
    MaxXYDistance = ExaktG::GMaxStrecke;

    for(int i = 0;i<ExaktG::MaxAxis;i++){
      Position[i]=0.0;
      AxisVelocity[i] = 200;
      setSpeedLevel(i,0);
    }

    SpeedLevelDistance[0] = ExaktG::G_pro_mm * ExaktG::SpeedDistance0in_mm;
    SpeedLevelDistance[1] = ExaktG::G_pro_mm * ExaktG::SpeedDistance1in_mm;
    SpeedLevelDistance[2] = ExaktG::G_pro_mm * ExaktG::SpeedDistance2in_mm;
    SpeedLevelDistance[3] = ExaktG::G_pro_mm * ExaktG::SpeedDistance3in_mm;
    SpeedLevelDistance[4] = ExaktG::G_pro_mm * ExaktG::SpeedDistance4in_mm;

    /*"posx":0.000} oder "posx":0.000,*/
    sScan.addScanner(nStreamScannerType_float,		\
		     (void*)this,			\
		     nG_posX,				\
		     (char*)"\"posx\":",		\
		     (char*)"}",(char*)",",		\
		     streamScanResult);

    /*"posy":0.000} oder "posy":0.000,*/
    sScan.addScanner(nStreamScannerType_float,				\
		     (void*)this,					\
		     nG_posY,						\
		     (char*)"\"posy\":",				\
		     (char*)"}",(char*)",",				\
		     streamScanResult);

    /*"posz":0.000} oder "posz":0.000,*/
    sScan.addScanner(nStreamScannerType_float,				\
		     (void*)this,					\
		     nG_posZ,						\
		     (char*)"\"posz\":",				\
		     (char*)"}",(char*)",",				\
		     streamScanResult);

    /*"posa":0.000} oder "posa":0.000,*/
    sScan.addScanner(nStreamScannerType_float,				\
		     (void*)this,					\
		     nG_posA,						\
		     (char*)"\"posa\":",				\
		     (char*)"}",(char*)",",				\
		     streamScanResult);
    /*"f":[1,0,4,4397]*/
    sScan.addScanner(nStreamScannerType_G_fReturn,	\
		     (void*)this,			\
		     nG_G_f,				\
		     (char*)"\"f\":[",			\
		     (char*)"]",			\
		     (char*)"]",			\
		     streamScanResult);
    /*"stat":x} */
    sScan.addScanner(nStreamScannerType_int,				\
		     (void*)this,					\
		     nG_stat,						\
		     (char*)"\"stat\":",				\
		     (char*)"}",(char*)"}",				\
		     streamScanResult);

  }

  void ExaktG::setFD(int fd)
  {
    this->pr_gcodes->setReadSource(fd,(char*)"ExaktG_Ctrl");
  }

  void ExaktG::setVerbose(int verb)
  {
    this->verbose = verb;    
  }

  G_Ctrl * ExaktG::getG_Ctrl(void)
  {
    return &GCtrl;
  }

  PollReader * ExaktG::getPollReader(void)
  {
    return this->pr_gcodes;
  }

  PollTimer * ExaktG::getPollTimer(void)
  {
    return this->pt_gcodes;
  }

  void ExaktG::setCoordinatesRelative(void)
  {
    GCtrl.cmdG((char*)"G91");
  }

  void ExaktG::setCoordinatesAbsolute(void)
  {
    GCtrl.cmdG((char*)"G90");
  }

  void ExaktG::setNullpunkt(int axis)
  {
    static char buf[128];
    if(ExaktG::AxisX==axis)
      {
	GCtrl.cmdG((char*)"G28.3 X0.0");
      }
    if(ExaktG::AxisY==axis)
      {
	snprintf(buf,128,"G28.3 Y%.0003f",MaxXYDistance);
	GCtrl.cmdG((char*)buf);
	//GCtrl.cmdG((char*)"G28.3 Y0.0");
      }
    if(ExaktG::AxisZ==axis)
      {
	GCtrl.cmdG((char*)"G28.3 Z0.0");
      }
    if(ExaktG::AxisA==axis)
      {
	GCtrl.cmdG((char*)"G28.3 A0.0");
      }
  }

  void ExaktG::pollTimerExpired(long time)
  {
    static int counter = 0;
    counter++;
    printf("ExaktG PollTimer Expired\n");
    if(counter==2)
      {
	printf("ExaktG PollTimer Zeit verändern\n");
	this->pt_gcodes->timeout = 500;
      }
    if(counter>=4)
      {
	printf("ExaktG PollTimer Zeit aus\n");
	this->pt_gcodes->timeout = -1;
      }
  }

  void ExaktG::pollReadEvent(PollSource * s)
  {
    char buf[1024];
    int buflen = 1024;
    int len = 0;

    memset(buf, 0, 1024);
    buf[1023] = '\n';

    len = read(s->thePollfd.fd,buf,buflen-1);
    if(len){
      for(int i = 0;i<len;i++){
	this->sScan.scan(buf[i]);
      }
      if(verbose){
	printf("%s",buf);
      }
    }
  }

  void ExaktG::setGCodeResultListener(struct ExaktG_CodeListener_t *pGCLis)
  {
    this->ptGCLis = pGCLis;
  }

  char * ExaktG::getSpeedText(int axis,int SpeedLevel)
  {
    static char SpeedTexte[ExaktG::MaxSpeedLevels][16];

    if(SpeedLevel<0){
      SpeedLevel = 0;
    }
    if(SpeedLevel>=ExaktG::MaxSpeedLevels){
      SpeedLevel = ExaktG::MaxSpeedLevels-1;
    }

    if(axis<0){
      axis = 0;
    }
    if(axis>=ExaktG::MaxAxis){
      SpeedLevel = ExaktG::MaxAxis-1;
    }

    snprintf(&SpeedTexte[SpeedLevel][0],		\
	     16,"%.003fmm",getAxisDistance(axis)/ExaktG::G_pro_mm);

    return &SpeedTexte[SpeedLevel][0];
  }

  void ExaktG::setSpeedLevel(int axis,int speed)
  {
    if(axis<0 || axis>=ExaktG::MaxAxis){
      return;
    }
    if(speed<0){
      speed = 0;
    }
    if(speed>=ExaktG::MaxSpeedLevels){
      speed = 0;
    }
    AxisSpeedLevel[axis] = speed;
  }

  void ExaktG::incSpeedLevel(int axis)
  {
    if(axis<0 || axis>=ExaktG::MaxAxis){
      return;
    }
    int speed = AxisSpeedLevel[axis];
    speed++;
    setSpeedLevel(axis,speed);
  }

  int ExaktG::getSpeedLevel(int axis)
  {
    if(axis<0 || axis>=ExaktG::MaxAxis){
      return 0;
    }
    return AxisSpeedLevel[axis];
  }

  float ExaktG::getXPos(void)
  {
    return Position[ExaktG::AxisX];
  }

  float ExaktG::getYPos(void)
  {
    return Position[ExaktG::AxisY];
  }

  float ExaktG::getZPos(void)
  {
    return Position[ExaktG::AxisZ];
  }

  float ExaktG::getAPos(void)
  {
    return Position[ExaktG::AxisA];
  }

  float ExaktG::getPos(int axis)
  {
    if(0>axis || axis<=ExaktG::MaxAxis){
      return 0.0;
    }else{
      return Position[axis];
    }
  }

  float ExaktG::getAxisDistance(int axis)
  {
     if(axis<0 || axis>=ExaktG::MaxAxis){
       return 0.0;
     }
     return SpeedLevelDistance[AxisSpeedLevel[axis]];
  }

  void ExaktG::holdMoving(bool holdIt)
  {
    this->isMovingHolded = holdIt;
  }

  void ExaktG::machineStateChangedEvent()
  {
    //machine state 3 = stop
    if((3 == this->MachineState)&&(this->isMovingHolded))
      {
	this->move(this->LastMovedAxis,AxisMoveDirection[LastMovedAxis]);
      }
  }

  void ExaktG::move(int axis,int direction)
  {
    float range;

    LastMovedAxis = axis;
    AxisMoveDirection[axis] = direction;

    if(axis<0 || axis>=ExaktG::MaxAxis){
      return;
    }
    range = getAxisDistance(axis);
    if(0>direction){
      range *=-1.0;
    }
    if(this->DistanceModeAbsolut)
      {
	range += Position[axis];
      }
    move(axis,range,AxisVelocity[axis],true);
  }

  void ExaktG::move(int axis,float targetPos,int axisVelocity,bool checkState)
  {
     bool doIt = true;

     if(checkState){
       //machine state 3 = stop; 1 = ready
       if(3==this->MachineState||1==MachineState){
	 doIt = true;
       }else{
	 doIt = false;
       }
     }

    if(doIt)
      {
	checkForCollision(axis,&targetPos);//hier wird range evtl. begrenzt
	this->GCtrl.cmdG1(axis,targetPos,axisVelocity);
      }
  }

  void ExaktG::setCollisionProtektion(bool cp)
  {
    this->collisionProtection = cp;
  }

  void ExaktG::checkForCollision(int axis,float * pTargetPos)
  {
    //Auch die X und die Y-Achse auf der selben Skala laufen lassen!!!
    //Schlitten X sitzt auf Null und Schlitten Y sitzt auf MaxPos!
    //Somit ist eine Kollision leichter zu berechnen
    if(ExaktG::AxisX==axis)
      {
	float YPos = this->getYPos();
	YPos -= this->SicherheitsAbstand;

	if(*pTargetPos>YPos)
	  {
	    *pTargetPos = YPos;
	  }
      }
    if(ExaktG::AxisY==axis)
      {
	float XPos = this->getXPos();
	XPos += this->SicherheitsAbstand;

	if(*pTargetPos<XPos)
	  {
	    *pTargetPos = XPos;
	  }
      }
  }

  void ExaktG::fastRecipeMove(float tarPosX,float tarPosY,float tarPosA)
  {
    float targetPos[4];
    
    targetPos[ExaktG::AxisX] = tarPosX;
    targetPos[ExaktG::AxisY] = tarPosY;
    targetPos[ExaktG::AxisA] = tarPosA;

    //A-Achse kann loslegen
    move(ExaktG::AxisZ,targetPos[ExaktG::AxisA],300,false);

    //Kollisionskontrolle muss hier sein, die in move funktioniert
    //nicht, wenn beide Achsen in Bewegung sind
    if(targetPos[ExaktG::AxisX]>targetPos[ExaktG::AxisY]-SicherheitsAbstand){
      targetPos[ExaktG::AxisX] = targetPos[ExaktG::AxisY]-SicherheitsAbstand;
    }

    if(targetPos[ExaktG::AxisY]<targetPos[ExaktG::AxisX]+SicherheitsAbstand){
      targetPos[ExaktG::AxisY] = targetPos[ExaktG::AxisX]+SicherheitsAbstand;
    }

    move(ExaktG::AxisX,targetPos[ExaktG::AxisX],300,false);
    move(ExaktG::AxisY,targetPos[ExaktG::AxisY],300,false);
  }

  void ExaktG::homeXandY(void)
  {
    this->GCtrl.cmdG1(ExaktG::AxisX,0.0,500,			\
		      ExaktG::AxisY,this->MaxXYDistance);
  }
}
