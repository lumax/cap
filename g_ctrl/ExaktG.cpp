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
      if(0!=pEG->ptGCLis->fnkXPosLis){
	(pEG->ptGCLis->fnkXPosLis)(pEG->ptGCLis->pTheListener,	\
				   ps->scannedFloat);
      }
      break;
    }
    case nG_posY:{
      if(0!=pEG->ptGCLis->fnkYPosLis){
	(pEG->ptGCLis->fnkYPosLis)(pEG->ptGCLis->pTheListener,	\
				   ps->scannedFloat);
      }
      break;
    }
    case nG_posZ:{
      if(0!=pEG->ptGCLis->fnkZPosLis){
	(pEG->ptGCLis->fnkZPosLis)(pEG->ptGCLis->pTheListener,	\
				   ps->scannedFloat);
      }
      break;
    }
    case nG_posA:{
      if(0!=pEG->ptGCLis->fnkAPosLis){
	(pEG->ptGCLis->fnkAPosLis)(pEG->ptGCLis->pTheListener,	\
				   ps->scannedFloat);
      }
      break;
    }
    case nG_G_f:{
      if(0!=pEG->ptGCLis->fnkGFLis){
	(pEG->ptGCLis->fnkGFLis)(pEG->ptGCLis->pTheListener,	\
				 ps->scannedG_F[0],		\
				 ps->scannedG_F[1],	\
				 ps->scannedG_F[2],	\
				 ps->scannedG_F[3]);
      }
      break;
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
}
