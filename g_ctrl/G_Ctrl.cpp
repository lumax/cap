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


#include "G_Ctrl.h"

namespace EuMax01
{

  G_Ctrl::~G_Ctrl(){}
  G_Ctrl::G_Ctrl(int verb)
  {
    this->setVerbose(verb);
    cmdLen = 64;
    fd = -1;
  }

  void G_Ctrl::setVerbose(int verb)
  {
    this->verbose = verb;    
  }

  char* G_Ctrl::getAxis(int axis)
  {
    switch(axis)
      {
      case nTinyG_Y:{
	return (char*)"Y";
      }
      case nTinyG_Z:{
	return (char*)"Z";
      }
      case nTinyG_A:{
	return (char*)"A";
      }
      default:{//nTinyG_X
	return (char*)"X";
      }
      };
  }

  void G_Ctrl::cmdFlowControl(void)
  {
    int ret = 0;
    char * flowControl = (char*)"{\"ex\":\"2\"}";
    ret = snprintf(this->cmdBuf,cmdLen,"%s\r",flowControl);
    if(0!=this->verbose)
      {
	printf("cmdFlowControl: %s\n",this->cmdBuf);
      }

    if(0<this->fd)
      {
	write(this->fd,this->cmdBuf,ret);
      }
  }

  void G_Ctrl::cmdG(char* gCode)
  {
    int ret = 0;

    ret = snprintf(this->cmdBuf,cmdLen,"%s\r",gCode);
    if(0!=this->verbose)
      {
	printf("cmdG1: %s\n",this->cmdBuf);
      }

    if(0<this->fd)
      {
	write(this->fd,this->cmdBuf,ret);
      }
  }

  void G_Ctrl::cmdG1(int axis,int range, int velocity)
  {
    int ret = 0;
    char * Axis = 0;

    Axis = getAxis(axis);

    ret = snprintf(this->cmdBuf,cmdLen,"G1 %s%i,F%i\r",\
		   Axis,range,velocity);
    if(0!=this->verbose)
      {
	printf("cmdG1: %s\n",this->cmdBuf);
      }

      if(0<this->fd)
      {
	write(this->fd,this->cmdBuf,ret);
      }
  }

  int G_Ctrl::getFd(void)
  {
    return this->fd;
  }

  void G_Ctrl::closeUart()
  {
    close(this->fd);
  }

  int G_Ctrl::openUart(char * path,int baud)
  {
    int     status = 0;
    struct termios newtio;

    // open the tty
    this->fd = open(path,O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (this->fd < 0)
      {
	if(this->verbose){
	  printf("G_Ctrl  openUart error opening uart fd:%i\n",fd);
	}
	return this->fd;
      }

    // flush serial port
    status = tcflush(this->fd, TCIFLUSH);
    if (status < 0)
      {
	if(this->verbose){
	  printf("G_Ctrl  openUart error tcflush\n");
	}
	close(this->fd);
	return -1;
      }
    /* get current terminal state */
    tcgetattr(this->fd,&newtio);
    cfmakeraw(&newtio);

    status = cfsetspeed(&newtio, baud);
    if (status < 0)
      {
	if(this->verbose){
	  printf("G_Ctrl  openUart error cfsetspeed\n");
	}
	close(this->fd);
	this->fd = -1;
	return this->fd;
      }

    newtio.c_cflag |=CRTSCTS; 

    // set its new attrigutes
    status = tcsetattr(this->fd,TCSANOW,&newtio);
    if (status < 0)
      {
	if(this->verbose){
	  printf("G_Ctrl  openUart error tcsetattr\n");
	}
	close(this->fd);
	this->fd = -1;
	return this->fd;
      }
    return this->fd;
  }
}
