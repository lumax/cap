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

  MBProtocol::MBProtocol()
  {
    fd = 0;
    lis = 0;
    isInit = false;
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
	  if(MBProt_class->lis)
	    MBProt_class->lis->FP1_evt(datum);
	  break;
	}
      case nPEC_GET_FP2:
	{
	  datum = getProtocol_UI16();
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
	if(MBProtocol::disableAuto())
	  perror("MBProtocoll::disableAuto failed on_exit\n");
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
