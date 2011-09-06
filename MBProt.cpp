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
#include "Dialoge.h"

#include "Tools.h"

#include "MBProt.h"

using namespace std;

namespace EuMax01
{

  MBProtocol::MBProtocol()
  {
    fd = 0;
  }

  int MBProtocol::initProtocol(GUI * pGUI)
  {
      // Open the tty:
    fd = open( "/dev/ttyACM0", O_RDWR );
    if (fd == -1)
    {
      return -1;
    }

    // Get the current options:
    if(tcgetattr( fd, &termOptions ))
      perror("error on tcgetattr\n");

    // Set the input/output speed to 921.6kbps
    cfsetispeed( &termOptions, B57600 );
    cfsetospeed( &termOptions, B57600 );

    // Now set the term options (set immediately)
    tcsetattr( fd, TCSANOW, &termOptions );

    pPollTimer = new PollTimer(5000,this);
    pPollIncoming = new PollReader(this);
    pGUI->addPollTimer(this->pPollTimer);
    this->pPollIncoming->setReadSource(fd);
    if(pGUI->addPollReader(pPollIncoming)!=0)
      {
	printf("addPollReader failed\n");
	close(fd);
	return -1;
      }
    return 0;
  }

  void MBProtocol::closeProtocol()
  {
    close(this->fd);
    this->fd = 0;
  }
  
  void MBProtocol::pollReadEvent(PollSource * s)
  {
    int ret;
    char buf[32];
    ret = read(fd,buf,32);
    printf("MBProtocol::pollReadEvent returned : %i\n",ret);
  }

  void MBProtocol::pollTimerExpired(long us)
  {
    printf("MBProtocoll Timer Expired\n");
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
}
