/*
Bastian Ruppert
*/
//read directories
#include <stdio.h>
#include <termios.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "Event.h"
#include "Button.h"
#include "Screen.h"
#include "Label.h"
#include "Globals.h"
#include "Poll.h"
#include "Main.h"
#include "Rezept.h"
#include "MenuBar.h"
#include "Dialoge.h"
#include "ArbeitsDialog.h"

#include "Tools.h"
#include "Protocol.h"
#include "pec_cmd.h"

#include "MBProt.h"

using namespace std;

namespace EuMax01
{
  static int MBProt_fd = 0;
  static MBProtocol * MBProt_class = 0;

  static const char * FP1_PATH = "/opt/capmb/FP1_last_position";
  static const char * FP2_PATH = "/opt/capmb/FP2_last_position";

  MBProtocol::MBProtocol()
  {
    fd = 0;
    lis = 0;
    isInit = false;
    lastPositionFP1 = 0;
    lastPositionFP2 = 0;
  }

  static void MBProt_putchar(unsigned char dat)
  {
    int ret = 0;
    ret=write(MBProt_fd,&dat,1);
    if(ret!=1)
      perror("MBProt_putchar: error writing data\n");
  }
  
  static unsigned short getProtocol_UI16(void)
  {
    unsigned short datum;
    unsigned char * pucQuelle = prt_get_data();
    datum = *(pucQuelle+1);//MSB
    datum <<=8;
    datum +=*pucQuelle;
    return datum;
  }

  static void MBProt_dispatcher(unsigned char ucDat)
  {
    static unsigned int counter = 0;
    unsigned short datum = 0;
    //printf("dispatcher ucDat: %i 0x%x %c\n",ucDat,ucDat,ucDat);
    switch(ucDat)
      {
      case nPEC_GET_Q1:
	{
	  datum = getProtocol_UI16();
	  if(MBProt_class->lis)
	    MBProt_class->lis->Q1_evt(datum);
	  break;
	}
      case nPEC_GET_Q2:
	{
	  datum = getProtocol_UI16();
	  if(MBProt_class->lis)
	    MBProt_class->lis->Q2_evt(datum);
	  break;
	}
      case nPEC_GET_Z1:
	{
	  datum = getProtocol_UI16();
	  if(MBProt_class->lis)
	    MBProt_class->lis->Z1_evt(datum);
	  break;
	}
      case nPEC_GET_Z2:
	{
	  datum = getProtocol_UI16();
	  if(MBProt_class->lis)
	    MBProt_class->lis->Z2_evt(datum);
	  break;
	}
      case nPEC_GET_FP1:
	{
	  datum = getProtocol_UI16();
	  //printf("dispatcherFP1: id :%x %i\n",ucDat,datum);
	  MBProt_class->setLastPositionFP1(datum);
	  if(MBProt_class->lis)
	    MBProt_class->lis->FP1_evt(datum);
	  break;
	}
      case nPEC_GET_FP2:
	{
	  datum = getProtocol_UI16();
	  MBProt_class->setLastPositionFP2(datum);
	  if(MBProt_class->lis)
	    MBProt_class->lis->FP2_evt(datum);
	  break;
	}
      case nPEC_SWVERSION:
	{
	  datum = getProtocol_UI16();
	  if(MBProt_class->lis)
	    MBProt_class->lis->SWVersion_evt(datum);
	  break;
	}
      case nPEC_HWVERSION:
	{
	  datum = getProtocol_UI16();
	  if(MBProt_class->lis)
	    MBProt_class->lis->HWVersion_evt(datum);
	  break;
	}
      case nPEC_KEYON://schwarzer Knopf
	{
	  exit(12);//shutdown
	  break;
	}
      case nPEC_KEYHELP://roter Knopf
	{
	  exit(11);//video
	  break;
	}
      default:
	{
	  counter++;
	  //printf("dispatcher ucDat: %i 0x%x %c\n",ucDat,ucDat,ucDat);
	}
      }
  }

  static void the_on_exit_fnc(int i, void * v)
  {
    if(MBProt_fd!=0)
      {
	if(MBProt_class!=0)
	  MBProt_class->saveLastPositionsToFile();

	if(MBProtocol::disableAuto())
	  perror("MBProtocoll::disableAuto failed on_exit\n");
	prt_sendmsg_int(nPEC_LIGHTOFF,0x0000);

	usleep(500000);
      }
      
    if(MBProt_class!=0)
      MBProt_class->closeProtocol();
  }

  int MBProtocol::initProtocol(GUI * pGUI,			\
			       IMBProtListener * listener,	\
			       char * device,			\
			       bool non_block)
  {
    int flags = 0;

    flags |= O_RDWR;

    if(non_block)
      flags |= O_NONBLOCK;

      // Open the tty:
    fd = open(device,flags);
    if (fd == -1)
    {
      return -1;
    }

    MBProt_fd = fd;

    // Get the current options:
    if(tcgetattr( fd, &termOptions ))
      perror("error on tcgetattr\n");

    cfmakeraw(&termOptions);

    // Set the input/output speed to 921.6kbps
    cfsetispeed( &termOptions, B57600 );
    cfsetospeed( &termOptions, B57600 );

    // Now set the term options (set immediately)
    tcsetattr( fd, TCSANOW, &termOptions );

    usleep(500);
    tcflush(MBProt_fd,TCIFLUSH);

    PRTDISPATCHER = MBProt_dispatcher;
    PRTPUTCH = MBProt_putchar;
    prtmodule_init();

    if(on_exit(the_on_exit_fnc,0))
       perror("MBProt set function on_exit failed!\n");

    pPollTimer = new PollTimer(100,this);
    pPollIncoming = new PollReader(this);
    pGUI->addPollTimer(this->pPollTimer);
    this->pPollIncoming->setReadSource(fd,(char*)"MBProt");

    if(pGUI->addPollReader(pPollIncoming)!=0)
      {
	printf("addPollReader failed\n");
	close(fd);
	return -1;
      }

    MBProt_class = this;
    this->lis = listener;

    this->isInit = true;
    return 0;
  }

  bool MBProtocol::isInitialised()
  {
    return isInit;
  }

  void MBProtocol::closeProtocol()
  {
    tcflush(MBProt_fd,TCIFLUSH);
    close(this->fd);
    this->fd = 0;
    MBProt_fd = 0;
    isInit = false;
  }
  
  void MBProtocol::pollReadEvent(PollSource * s)
  {
    int ret;
    int i;
    char buf[32];
    ret = read(fd,buf,32);
    //printf("MBProtocol::pollReadEvent returned : %i\n",ret);
    for(i=0;i<ret;i++)
      {
	PRTPROCESS(buf[i]);
      }
  }

  void MBProtocol::pollTimerExpired(long us)
  {
    prt_timer();
  }

  void MBProtocol::getLastPositionsFromFile(int sdlw,int camw)
  {
    int fd = 0;
    char buf[128];
    //MitteCrossCamX nach ArbeitsDialog berechnet
    int MitteCrossCam=0;

    if(sdlw/2<camw)
      MitteCrossCam=sdlw/4;
    else//sdlw/2>camw
      MitteCrossCam=sdlw/4;

    fd = open(FP1_PATH,O_RDONLY);
    if(-1!=fd)
      {
	if(read(fd,buf,128)!=-1)
	  {
	    this->lastPositionFP1 =  atoi(buf);
	  }
	else
	  {
	    printf("error reading %s\n",FP1_PATH);
	    this->lastPositionFP1 = MitteCrossCam;
	  }
	close(fd);
      }
    else
      {
	printf("error opening %s, using standard FP1\n",FP1_PATH);
	this->lastPositionFP1 = MitteCrossCam;
      }

    fd = open(FP2_PATH,O_RDONLY);
    if(-1!=fd)
      {
	if(read(fd,buf,128)!=-1)
	  {
	    this->lastPositionFP2 =  atoi(buf);
	  }
	else
	  {
	    printf("error reading %s\n",FP2_PATH);
	    this->lastPositionFP2 = MitteCrossCam;
	  }
	close(fd);
      }
    else
      {
	printf("error opening %s, using standard FP2\n",FP2_PATH);
	this->lastPositionFP2 = MitteCrossCam;
      }
  }

  void MBProtocol::saveLastPositionsToFile()
  {
    FILE * fd = 0;
    char buf[128];
    int ret = 0;
    fd = fopen(FP1_PATH,"w");
    if(0!=fd)
      {
	ret = snprintf(buf,128,"%i",this->lastPositionFP1);
	if(ret>0)
	  {
	    if(fputs(buf,fd)<=-1)
	      {
		printf("error writing %s\n",FP1_PATH);
	      }
	  }
	else
	  {
	    printf("error creating Buffer for writing %s\n",FP1_PATH);
	  }
	fclose(fd);
      }
    else
      {
	printf("error opening %s, for writing\n",FP1_PATH);
      }

    fd = fopen(FP2_PATH,"w");
    if(0!=fd)
      {
	ret = snprintf(buf,128,"%i",this->lastPositionFP2);
	if(ret>0)
	  {
	    if(fputs(buf,fd)<=-1)
	      {
		printf("error writing %s\n",FP2_PATH);
	      }
	  }
	else
	  {
	    printf("error creating Buffer for writing %s\n",FP2_PATH);
	  }
	fclose(fd);
      }
    else
      {
	printf("error opening %s, for writing\n",FP2_PATH);
      }
  }

  int MBProtocol::getLastPositionFP1()
  {
    return lastPositionFP1;
  }

  int MBProtocol::getLastPositionFP2()
  {
    return lastPositionFP2;
  }

  void MBProtocol::setLastPositionFP1(unsigned short dat)
  {
    lastPositionFP1 = dat;
  }

  void MBProtocol::setLastPositionFP2(unsigned short dat)
  {
    lastPositionFP2 = dat;
  }

  int MBProtocol::getQ1()
  {
    char buf[7];
    buf[0]=0x1f;
    buf[1]=0x34;
    buf[2]=0x07;
    buf[3]=0x01;
    buf[4]=0x00;
    buf[5]=0x00;
    buf[6]=0x21;
    if(write(fd,buf,7)!=7)
      return -1;
    else
      return 0;
  }
  int MBProtocol::disableAuto()
  {
    char buf[7];
    buf[0]=0x1f;
    buf[1]=0x22;
    buf[2]=0x07;
    buf[3]=0x0a;
    buf[4]=0x00;
    buf[5]=0x00;
    buf[6]=0x35;
    if(write(MBProt_fd,buf,7)!=7)
      return -1;
    else
      return 0;
  }

  int MBProtocol::enableAuto()
  {
    char buf[7];
    buf[0]=0x1f;
    buf[1]=0x20;
    buf[2]=0x07;
    buf[3]=0x01;
    buf[4]=0x00;
    buf[5]=0x00;
    buf[6]=0xe7;
    if(write(fd,buf,7)!=7)
      return -1;
    else
      return 0;
  }

}
