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
    cmdLen = 128;
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

  void G_Ctrl::sendCmd(char * cmd)
  {
    int ret;
    ret = snprintf(this->cmdBuf,this->cmdLen,"%s\r",cmd);
    if(0<this->fd)
      {
	write(this->fd,this->cmdBuf,ret);
      }   
  }

  //Die Einstellungen für die Achsen müssten anscheinend nicht
  //immer erneut erfolgen.
  //Im G_TestDialog mit $$ alle Infos vom TinyG holen.
  //Im G_TestDialog eine geschweifte Klammer mit alt+altgr+7
  void G_Ctrl::cmdGetStatus(void)
  {
    //[1pm] m1 power management 1 [0=disabled,1=always on,2=in cycle,3=when moving]
    //X-Achse steht auf 1, alle anderen Achsen stehen auf 2!!!
    //{"1tr":1.500}
    char * getStatus = (char*)"{\"sr\":\"\"}";
    char * motorEnable = (char*)"{\"me\":\"\"}";
    char * TravelPerRevolution1 = (char*)"{\"1tr\":1.4994\"}";
    char * TravelPerRevolution2 = (char*)"{\"2tr\":1.4994\"}";
    char * XAchsePowerInCylce = (char*)"{\"1pm\":1\"}";
    char * YAchsePowerInCylce = (char*)"{\"2pm\":1\"}";
    char * AAchsePowerInCylce = (char*)"{\"4pm\":1\"}";
    char * ZAchsePowerInCylce = (char*)"{\"3pm\":1\"}";
    char * AAchseMoveInDegrees = (char*)"{\"aam\":1\"}";
    //char * TravelPerRevolution4 = (char*)"{\"4tr\":4\"}";
    sendCmd(getStatus);
    sendCmd(motorEnable);
    sendCmd(TravelPerRevolution1);
    sendCmd(TravelPerRevolution2);
    //   sendCmd(XAchsePowerInCylce);
    //sendCmd(YAchsePowerInCylce);
    //sendCmd(AAchsePowerInCylce);
    //sendCmd(ZAchsePowerInCylce);
    //sendCmd(AAchseMoveInDegrees);
    //sendCmd(TravelPerRevolution4);
    sendCmd(getStatus);
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
    cmdG1(axis,(float)range,velocity);
  }

  void G_Ctrl::cmdG1(int axis,float range, int velocity,\
		     int axis2,float range2)
  {
    int ret = 0;
    char * pcAxis = 0;
    char * pcAxis2 = 0;

    pcAxis = getAxis(axis);
    pcAxis2 = getAxis(axis2);

    ret = snprintf(this->cmdBuf,cmdLen,"G1 %s%.0003f,%s%.0003f,F%i\r",\
		   pcAxis,range,pcAxis2,range2,velocity);
    if(0!=this->verbose)
      {
	printf("cmdG1: %s\n",this->cmdBuf);
      }

      if(0<this->fd)
      {
	write(this->fd,this->cmdBuf,ret);
      }
  }

  void G_Ctrl::cmdG1(int axis,float range, int velocity,\
		     int axis2,float range2,int axis3, float range3)
  {
    int ret = 0;
    char * pcAxis = 0;
    char * pcAxis2 = 0;
    char * pcAxis3 = 0;

    pcAxis = getAxis(axis);
    pcAxis2 = getAxis(axis2);
    pcAxis3 = getAxis(axis3);

    ret = snprintf(this->cmdBuf,cmdLen,				\
		   "G1 %s%.0003f,%s%.0003f,,%s%.0003fF%i\r",	\
		   pcAxis,range,pcAxis2,range2,pcAxis3,range3,velocity);
    if(0!=this->verbose)
      {
	printf("cmdG1: %s\n",this->cmdBuf);
      }

      if(0<this->fd)
      {
	write(this->fd,this->cmdBuf,ret);
      }
  }

  void G_Ctrl::cmdG1(int axis,float range, int velocity)
  {
    int ret = 0;
    char * Axis = 0;

    Axis = getAxis(axis);

    ret = snprintf(this->cmdBuf,cmdLen,"G1 %s%.0003f,F%i\r",\
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
