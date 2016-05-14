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
    printf("StreamScanner Result Int: %i Float: %f\n",	\
	   ps->scannedInt, ps->scannedFloat);
  }

  ExaktG::ExaktG(int verbExakt,int verbG):GCtrl(verbG)
  {
    this->setVerbose(verbExakt);
    pr_gcodes = new PollReader(this);
    pt_gcodes = new PollTimer(-1,this);

    /*"posx":0.000} oder "posx":0.000,*/
    sScan.addScanner(nStreamScannerType_float,		\
		     (void*)this,			\
		     nTinyG_X,				\
		     (char*)"\"posx\":",		\
		     (char*)"}",(char*)",",		\
		     streamScanResult);

    /*"posy":0.000} oder "posy":0.000,*/
    sScan.addScanner(nStreamScannerType_float,				\
		     (void*)this,					\
		     nTinyG_Y,						\
		     (char*)"\"posy\":",				\
		     (char*)"}",(char*)",",				\
		     streamScanResult);

    /*"posz":0.000} oder "posz":0.000,*/
    sScan.addScanner(nStreamScannerType_float,				\
		     (void*)this,					\
		     nTinyG_Y,						\
		     (char*)"\"posz\":",				\
		     (char*)"}",(char*)",",				\
		     streamScanResult);

    /*"posa":0.000} oder "posa":0.000,*/
    sScan.addScanner(nStreamScannerType_float,				\
		     (void*)this,					\
		     nTinyG_Y,						\
		     (char*)"\"posa\":",				\
		     (char*)"}",(char*)",",				\
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
}
