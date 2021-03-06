/*
Bastian Ruppert
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <SDL/SDL_ttf.h>
#include <errno.h>
#include "LL.h"
#include "Event.h"
#include "Tools.h"
#include "Globals.h"
#include "Button.h"
#include "Poll.h"
#include "Label.h"
#include "Screen.h"
#include "Main.h"
#include "iniParser.h"

#include "Rezept.h"

namespace EuMax01
{

  /*static void evtB1(void * src,SDL_Event * evt){
  cap_cam_addCrossX(0,-10);
}

static void evtExit(void * src,SDL_Event * evt){
  GUI::getInstance(0,0)->stopEventLoop();
  }*/

  Rezept::Rezept()
  {
    int i;
    for(i=0;i<Rezept::AnzahlRezepte;i++)
      {
	this->Rezepte[i].cams[0].x_pos = 0;
	this->Rezepte[i].cams[0].z_pos = 0;
	this->Rezepte[i].cams[0].x_cross = 0;
	this->Rezepte[i].cams[0].diameter = Rezept::DefaultDiameter;
	this->Rezepte[i].cams[0].walze = Rezept::DefaultWalze;

	this->Rezepte[i].cams[1].x_pos = 0;
	this->Rezepte[i].cams[1].z_pos = 0;
	this->Rezepte[i].cams[1].x_cross = 0;
	this->Rezepte[i].cams[1].diameter = Rezept::DefaultDiameter;
	this->Rezepte[i].cams[1].walze = Rezept::DefaultWalze;

	this->Rezepte[i].GPosition.PosGAxisX = 0.0;
	this->Rezepte[i].GPosition.PosGAxisY = 0.0;
	this->Rezepte[i].GPosition.PosGAxisA = 0.0;
	this->Rezepte[i].GPosition.PosGAxisZ = 0.0;
      }

    for(i=0;i<Rezept::NameLength;i++)
      {
	this->Name[i] = '\0';
      }
    this->Name[0] = ' ';
  };

  void Rezept::copy(Rezept * source,Rezept * target)
  {
    int i;
    for(i=0;i<Rezept::AnzahlRezepte;i++)
      {
	target->Rezepte[i].cams[0].x_pos = source->Rezepte[i].cams[0].x_pos;
	target->Rezepte[i].cams[0].z_pos = source->Rezepte[i].cams[0].z_pos;
	target->Rezepte[i].cams[0].x_cross = source->Rezepte[i].cams[0].x_cross;
	target->Rezepte[i].cams[0].diameter = source->Rezepte[i].cams[0].diameter;
	target->Rezepte[i].cams[0].walze = source->Rezepte[i].cams[0].walze;

	target->Rezepte[i].cams[1].x_pos = source->Rezepte[i].cams[1].x_pos;
	target->Rezepte[i].cams[1].z_pos = source->Rezepte[i].cams[1].z_pos;
	target->Rezepte[i].cams[1].x_cross = source->Rezepte[i].cams[1].x_cross;
	target->Rezepte[i].cams[1].diameter = source->Rezepte[i].cams[1].diameter;
	target->Rezepte[i].cams[1].walze = source->Rezepte[i].cams[1].walze;

	target->Rezepte[i].GPosition.PosGAxisX = source->Rezepte[i].GPosition.PosGAxisX;
	target->Rezepte[i].GPosition.PosGAxisY = source->Rezepte[i].GPosition.PosGAxisY;
	target->Rezepte[i].GPosition.PosGAxisA = source->Rezepte[i].GPosition.PosGAxisA;
	target->Rezepte[i].GPosition.PosGAxisZ = source->Rezepte[i].GPosition.PosGAxisZ;
      }

    for(i=0;i<Rezept::NameLength;i++)
      {
	target->Name[i] = source->Name[i];
      }
  }

  Rezept::~Rezept()
  {

  }

  struct GPosition_t Rezept::getGPositions(int rezept)
  {
    int pos;

    if(rezept<0)
      pos = 0;
    else if(rezept>=Rezept::AnzahlRezepte)
      pos = Rezept::AnzahlRezepte-1;
    else
      pos = rezept;

    return this->Rezepte[pos].GPosition;
  }

  unsigned int Rezept::getXPosition(int cam,int rezept)
  {
    int theCam = 0;
    int pos;

    if(cam)
      theCam = 1;

    if(rezept<0)
      pos = 0;
    else if(rezept>=Rezept::AnzahlRezepte)
      pos = Rezept::AnzahlRezepte-1;
    else
      pos = rezept;
    
    return this->Rezepte[pos].cams[theCam].x_pos;
  }

  unsigned int Rezept::getZPosition(int rezept)
  {
    int theCam = 0;
    int pos;

    if(rezept<0)
      pos = 0;
    else if(rezept>=Rezept::AnzahlRezepte)
      pos = Rezept::AnzahlRezepte-1;
    else
      pos = rezept;
    
    return this->Rezepte[pos].cams[theCam].z_pos;
  }

  int Rezept::getDiameter(int rezept)
  {
    int theCam = 0;
    int pos;

    if(rezept<0)
      pos = 0;
    else if(rezept>=Rezept::AnzahlRezepte)
      pos = Rezept::AnzahlRezepte-1;
    else
      pos = rezept;

    return this->Rezepte[pos].cams[theCam].diameter;
  }

  int Rezept::getWalze(int rezept)
  {
    int theCam = 0;
    int pos;

    if(rezept<0)
      pos = 0;
    else if(rezept>=Rezept::AnzahlRezepte)
      pos = Rezept::AnzahlRezepte-1;
    else
      pos = rezept;

    return this->Rezepte[pos].cams[theCam].walze;
  }
  
  int Rezept::writeToFile(char * SaveDir)
  {
    int fd,ret,i;

    i = mkdir(SaveDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(i<0){
      if(errno!=EEXIST)
	perror("mkdir in writeToFile\n");
    }

    snprintf(this->buf,1024,"%s%s",SaveDir,this->Name);
    fd = open(buf,O_RDWR | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    if(fd==-1)
      {
	perror("Rezept::WriteToFile open failed!\n");
	return -1;
      }
    sprintf(this->buf,"#Rezept File Version: %i\n",this->version);
    ret = write(fd,buf,strlen(buf));
    if(ret==-1)
      goto error_out;

    for(i=0;i<8;i++)
      {
	sprintf(this->buf,"r%i_cam1_x_pos = %i\n",i,this->Rezepte[i].cams[0].x_pos);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_cam1_z_pos = %i\n",i,this->Rezepte[i].cams[0].z_pos);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_cam1_x_cross = %i\n",i,this->Rezepte[i].cams[0].x_cross);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_cam1_diameter = %i\n",i,this->Rezepte[i].cams[0].diameter);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_cam1_walze = %i\n",i,this->Rezepte[i].cams[0].walze);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;

	sprintf(this->buf,"r%i_cam2_x_pos = %i\n",i,this->Rezepte[i].cams[1].x_pos);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_cam2_z_pos = %i\n",i,this->Rezepte[i].cams[1].z_pos);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_cam2_x_cross = %i\n",i,this->Rezepte[i].cams[1].x_cross);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_cam2_diameter = %i\n",i,this->Rezepte[i].cams[1].diameter);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_cam2_walze = %i\n",i,this->Rezepte[i].cams[1].walze);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;

	sprintf(this->buf,"r%i_GPositionX = %.0003f\n",i,	\
		this->Rezepte[i].GPosition.PosGAxisX);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_GPositionY = %.0003f\n",i,	\
		this->Rezepte[i].GPosition.PosGAxisY);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_GPositionZ = %.0003f\n",i,	\
		this->Rezepte[i].GPosition.PosGAxisZ);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;
	sprintf(this->buf,"r%i_GPositionA = %.0003f\n",i,	\
		this->Rezepte[i].GPosition.PosGAxisA);
	if(write(fd,buf,strlen(buf))==-1)
	  goto error_out;	
      }

    close(fd);
    return 0;
  error_out:
    perror("Rezept::WriteToFile write failed!\n");
    close(fd);
    return -1;
  }

  int Rezept::readFromFile(char * FilePath,char * FileName)
  {
    char tmpBuf[1024];
    char tmp[64];

    snprintf(tmpBuf,1024,"%s%s",FilePath,FileName);

    for(int i=0;i<Rezept::AnzahlRezepte;i++)
      {
	sprintf(this->buf,"r%i_cam1_x_pos",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    goto error_out;
	  }
	else
	  {
	    this->Rezepte[i].cams[0].x_pos = atoi(tmp);
	  }
	
	sprintf(this->buf,"r%i_cam1_z_pos",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    goto error_out;
	  }
	else
	  {
	    this->Rezepte[i].cams[0].z_pos = atoi(tmp);
	  }
	
	sprintf(this->buf,"r%i_cam1_x_cross",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    goto error_out;
	  }
	else
	  {
	    this->Rezepte[i].cams[0].x_cross = atoi(tmp);
	  }

	sprintf(this->buf,"r%i_cam1_diameter",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    this->Rezepte[i].cams[0].diameter = Rezept::DefaultDiameter;
	  }
	else
	  {
	    this->Rezepte[i].cams[0].diameter = atoi(tmp);
	  }

	sprintf(this->buf,"r%i_cam1_walze",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    this->Rezepte[i].cams[0].walze = Rezept::DefaultWalze;
	  }
	else
	  {
	    this->Rezepte[i].cams[0].walze = atoi(tmp);
	  }

	sprintf(this->buf,"r%i_cam2_x_pos",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    goto error_out;
	  }
	else
	  {
	    this->Rezepte[i].cams[1].x_pos = atoi(tmp);
	  }

	sprintf(this->buf,"r%i_cam2_z_pos",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    goto error_out;
	  }
	else
	  {
	    this->Rezepte[i].cams[1].z_pos = atoi(tmp);
	  }

	sprintf(this->buf,"r%i_cam2_x_cross",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    goto error_out;
	  }
	else
	  {
	    this->Rezepte[i].cams[1].x_cross = atoi(tmp);
	  }

	sprintf(this->buf,"r%i_cam2_diameter",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    this->Rezepte[i].cams[1].diameter = Rezept::DefaultDiameter;
	  }
	else
	  {
	    this->Rezepte[i].cams[1].diameter = atoi(tmp);
	  }
	sprintf(this->buf,"r%i_cam2_walze",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    this->Rezepte[i].cams[1].walze = Rezept::DefaultWalze;
	  }
	else
	  {
	    this->Rezepte[i].cams[1].walze = atoi(tmp);
	  }

	sprintf(this->buf,"r%i_GPositionX",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    this->Rezepte[i].GPosition.PosGAxisX = 0.0;
	  }
	else
	  {
	    this->Rezepte[i].GPosition.PosGAxisX = (float)atof(tmp);
	  }

	sprintf(this->buf,"r%i_GPositionY",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    this->Rezepte[i].GPosition.PosGAxisY = 0.0;
	  }
	else
	  {
	    this->Rezepte[i].GPosition.PosGAxisY = (float)atof(tmp);
	  }

	sprintf(this->buf,"r%i_GPositionZ",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    this->Rezepte[i].GPosition.PosGAxisZ = 0.0;
	  }
	else
	  {
	    this->Rezepte[i].GPosition.PosGAxisZ = (float)atof(tmp);
	  }

	sprintf(this->buf,"r%i_GPositionA",i);
	if(iniParser_getParam(tmpBuf,this->buf,tmp,64))
	  {
	    this->Rezepte[i].GPosition.PosGAxisA = 0.0;
	  }
	else
	  {
	    this->Rezepte[i].GPosition.PosGAxisA = (float)atof(tmp);
	  }
      }

    memcpy(this->Name,FileName,Rezept::NameLength);

    return 0;
  error_out:
    return -1;
  }
}
