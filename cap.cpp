//#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
//#include <iostream>
//#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "Event.h"
#include "Button.h"
//#include <fsgCheckBox.h>
//#include <fsgLabel.h>
#include "Screen.h"
#include "Label.h"
#include "Globals.h"
#include "Poll.h"
#include "Main.h"
#include "Tools.h"
#include "Rezept.h"
#include "MenuBar.h"
#include "Dialoge.h"
#include "ArbeitsDialog.h"
#include "pec_cmd.h"
#include "MBProt.h"
#include "Protocol.h"
#ifdef EUMAX01_MJPEG_SUPPORT
#include <dsp_jpeg.h>
#endif
#include "Version.h"
#include "v4l_capture.h"
#include <linux/videodev.h>
#include "V4L2_utils.h"
#include "iniParser.h"

//#include "dsp_color.h"

using namespace std;
using namespace EuMax01;

static void oneSecondTimer(void);

class CamControl:IPollTimerListener,IPollReadListener
{
public:
  CamControl(GUI * pGUI,int PixelFormat,bool RGB_mode,int yBottomSide);
  virtual void pollReadEvent(PollSource * s);
  virtual void pollTimerExpired(long us);
  unsigned char *framebuffer0;
  unsigned char *framebuffer1;
private:
  int yBottomSide;
  bool RGB_Mode;
  bool cam0ready;
  bool cam1ready;
  PollTimer * pPollTimer;
  PollReader * pPollReaderCam0;
  PollReader * pPollReaderCam1;
  GUI * ptheGUI;
  int PixelFormat;//0 = normal, 1 = MJPEG
};

static int camwidth = 0;
static int camheight = 0;
static int sdlwidth = 1024;
static int sdlheight = 576;

static CamControl * camCtrl;

static unsigned char getYfromRGB(unsigned char r,unsigned char g,unsigned char b)
{
  //double d = ((0.257*r)+(0.504*g)+(0.098*(double)b)+16);
  //return (unsigned char)d;
  return (unsigned char)((0.257*r)+(0.504*g)+(0.098*b)+16);
}

static unsigned char getUfromRGB(unsigned char r,unsigned char g,unsigned char b)
{
  return (unsigned char)((0.439*r)-(0.368*g)-(0.071*b)+128);
}

static unsigned char getVfromRGB(unsigned char r,unsigned char g,unsigned char b)
{
  return (unsigned char)(-(0.148*r)-(0.291*g)+(0.439*b)+128);
}

static void overlayAndCrossair(struct v4l_capture* cap,char * pc,size_t len)
{
  unsigned int i,ii;
  unsigned int w = (unsigned int)cap->camWidth;
  unsigned int h = (unsigned int)cap->camHeight;
  int alles = 0;
  int cam = cap->camnumber;
  int wMalZwei = w*2;
  int wMalVier = w*4;
  int offset = cam*wMalZwei;

  //Fadenkreuz
  unsigned int crossBreite = cap->camWidth;
  unsigned int crossDicke = 1;
  int zeile = w*2;
  //unsigned int crossX = w/2-crossBreite/2;
  //unsigned int crossY = h/2-crossDicke;
  unsigned int crossX = cap->camCrossX;
  unsigned int crossY = cap->camHeight/2;

  int start = zeile*crossY;
  static int flag = 1;
  static unsigned char Y = 106,U = 221,V = 202;


    if(flag)
    {
      Y = getYfromRGB(0,255,0);//gruen
      U = getUfromRGB(0,255,0);
      V = getVfromRGB(0,255,0);
      //Y = getYfromRGB(0xfe,0x2e,0xf7);//lila
      //U = getUfromRGB(0xfe,0x2e,0xf7);
      //V = getVfromRGB(0xfe,0x2e,0xf7);
      printf("Y = %i, U=%i, V=%i\n",Y,U,V);
      flag=0;
      }

  //int lineoffset = crossY*h*4;

  //horizontale Linie
  for(i=0;i<crossDicke;i++)
    {
      for(ii=0;ii<crossBreite*2;ii+=4)//ii++)
	{
	  //pc[start+ii]=0x00;
	  pc[start+ii]=Y;
	  pc[start+ii+1]=V;
	  pc[start+ii+2]=Y;
	  pc[start+ii+3]=U;
	}
      start+=zeile;
    }
  //vertikale Linie
  start = (crossX*2);//+zeile*(crossBreite/2);
  for(i=0;i<h;i++)
    {
      pc[start+ii]=Y;
      pc[start+ii+1]=V;
      pc[start+ii+2]=Y;
      pc[start+ii+3]=U;
      start+=zeile;
    }

  for(i=0;i<h;i++)
    {
      memcpy(cap->sdlOverlay->pixels[0]+i*wMalVier+offset,	\
	     pc+alles,						\
	     wMalZwei);
	  alles += w*2;
    }
}

/***************************************************************/
static void processMJPEG(struct v4l_capture* cap,const void * p,int method,size_t len)
{
  static int counter =0;
  unsigned char *framebuffer;
#ifdef EUMAX01_MJPEG_SUPPORT
  int i;
#endif

#ifndef EUMAX01_MJPEG_SUPPORT
  printf("NO EUMAX01_MJPEG_SUPPORT\n");
  return;
#endif
  //if(cap->camnumber)
  //  return;
  if(counter<=50)
    {
      counter++;
      return;
    }
  if(method==IO_METHOD_MMAP)
    {
      //printf("pixelformat = MJPEG\n");

      if(cap->camnumber)
	{
	  framebuffer = camCtrl->framebuffer1;
	}
      else
	{
	  framebuffer = camCtrl->framebuffer0;
	}
#ifdef EUMAX01_MJPEG_SUPPORT
      i = jpeg_decode(framebuffer,(unsigned char*)p,\
		      &cap->camWidth,\
		      &cap->camHeight);
      if(i)
	printf("error jpeg_decode\n");
#endif
      SDL_LockSurface(cap->mainSurface);
      SDL_LockYUVOverlay(cap->sdlOverlay);

      overlayAndCrossair(cap,(char *)framebuffer,len);

      SDL_UnlockYUVOverlay(cap->sdlOverlay);
      SDL_UnlockSurface(cap->mainSurface);

      /*tmpRect.h=tmpRect.h*MULTIPLIKATOR;*2;//untereinander
	tmpRect.w=tmpRect.w*2*MULTIPLIKATOR;*4;//nebeneinander*/
      SDL_DisplayYUVOverlay(cap->sdlOverlay, &cap->sdlRect);
    }
  else if(method==IO_METHOD_USERPTR)
    {
      printf("IO_METHOD_USEPTR not supported in process_image2\n");
    }
  else
    {
      fputc ('_', stdout);
      fflush (stdout);
    }
}
/***************************************************************/
static void processImages(struct v4l_capture* cap,const void * p,int method,size_t len)
{
  if(method==IO_METHOD_MMAP)
    {
      SDL_LockSurface(cap->mainSurface);
      SDL_LockYUVOverlay(cap->sdlOverlay);

      char * pc = (char *)p;
      overlayAndCrossair(cap,(char *)pc,len);

      //printf("alles = %i, len = %i\n",alles,len);
      SDL_UnlockYUVOverlay(cap->sdlOverlay);
      SDL_UnlockSurface(cap->mainSurface);

      /*tmpRect.h=tmpRect.h*MULTIPLIKATOR;*2;//untereinander
	tmpRect.w=tmpRect.w*2*MULTIPLIKATOR;*4;//nebeneinander*/
      SDL_DisplayYUVOverlay(cap->sdlOverlay, &cap->sdlRect);
    }
  else if(method==IO_METHOD_USERPTR)
    {
      printf("IO_METHOD_USEPTR not supported in process_image2\n");
    }
  else
    {
      fputc ('_', stdout);
      fflush (stdout);
    }
}

/*static inline unsigned char clamp (double x)
{
int r = x;      // round to nearest

        if (r < 0)         return 0;
        else if (r > 255)  return 255;
        else               return r;
}

static void inverseConversion()
{
  int Y1, Cb, Cr;         // gamma pre-corrected input [0;255]
int ER, EG, EB;         // output [0;255]

 double r, g, b;         // temporaries
  double y1, pb, pr;

  y1 = (255 / 219.0) * (Y1 - 16);
  pb = (255 / 224.0) * (Cb - 128);
  pr = (255 / 224.0) * (Cr - 128);

  r = 1.0 * y1 + 0     * pb + 1.402 * pr;
  g = 1.0 * y1 - 0.344 * pb - 0.714 * pr;
  b = 1.0 * y1 + 1.772 * pb + 0     * pr;

  ER = clamp (r * 255); // [ok? one should prob. limit y1,pb,pr]
  EG = clamp (g * 255);
  EB = clamp (b * 255);
}
*/
static unsigned char Rtable[256][256];
static unsigned char Gtable[256][256][256];
static unsigned char Btable[256][256];
static void initRGBtables(void);
static unsigned char getRfromTable(unsigned char Y,unsigned char V);
static unsigned char getGfromTable(unsigned char Y,unsigned char U,unsigned char V);
static unsigned char getBfromTable(unsigned char Y,unsigned char U);
static void initRGBtables(void)
{
#ifndef EUMAX01_MJPEG_SUPPORT
  printf("NO EUMAX01_MJPEG_SUPPORT\n");
  return;
#endif
#ifdef EUMAX01_MJPEG_SUPPORT
  int i,ii,iii;
  for(i=0;i<256;i++)
    {
      for(ii=0;ii<256;ii++)
	{
	  Rtable[i][ii]=R_FROMYV(i,ii);
	}
    }
  for(i=0;i<256;i++)
    {
      for(ii=0;ii<256;ii++)
	{
	  for(iii=0;iii<256;iii++)
	    {
	       Gtable[i][ii][iii]=G_FROMYUV(i,ii,iii);
	    }
	}
    }
 for(i=0;i<256;i++)
    {
      for(ii=0;ii<256;ii++)
	{
	  Btable[i][ii]=B_FROMYU(i,ii);
	}
    }
#endif
}
static inline unsigned char getRfromTable(unsigned char Y,unsigned char V)
{
  return Rtable[Y][V];
}

static inline unsigned char getGfromTable(unsigned char Y,unsigned char U,unsigned char V)
{
  return Gtable[Y][U][V];
}

static inline unsigned char getBfromTable(unsigned char Y,unsigned char U)
{
  return Btable[Y][U];
}

static void convertYUYVtoRGB(char * src,char * target,int w,int h)
{
  //R = 1.164(Y - 16) + 1.596(V - 128)
  //G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
  //B = 1.164(Y - 16) + 2.018(U - 128)

  //target : RGBA
  //source Y0 U0 Y1 V0  Y2 U2 Y3 V2  ...usw.
  //Es gibt w*h Pixel! YUYV hat zwei Pixel in vier Bytes

  unsigned int len = (w*h)/2;
  unsigned int i;
  //double term1 = 0;
  //char alpha = 0;//255;//128;

  char * ps = src;
  char * pt = target;

#define rYa ps[0]
#define rYb ps[2]
#define rU  ps[1]
#define rV  ps[3]

  for(i=0;i<len;i++)
    {
      /*      term1 = (double)(1.164*(ps[0]-16));
      pt[2] = (char)( term1 + 1.596*(ps[3]-128) );//R
      pt[1] = (char)( term1 - 0.813*(ps[3]-128) - 0.391*(ps[1] - 128) );//G
      pt[0] = (char)( term1 + 2.018*(ps[1]-128) );//B
      pt[3] = alpha;
      */
      /*
      pt[2]=R_FROMYV(rYa,rV);
      pt[1]=G_FROMYUV(rYa,rU,rV);
      pt[0]=B_FROMYU(rYa,rU);
      */
      /*     pt[2]=Rtable[rYa][rV];//getRfromTable(rYa,rV);
      pt[1]=Gtable[rYa][rU][rV];//getGfromTable(rYa,rU,rV);
      pt[0]=Btable[rYa][rU];//getBfromTable(rYa,rU);
      */
      /*
      pt[2]=Rtable[ps[0]][ps[3]];//getRfromTable(rYa,rV);
      pt[1]=Gtable[ps[0]][ps[1]][ps[3]];//getGfromTable(rYa,rU,rV);
      pt[0]=Btable[ps[0]][ps[1]];//getBfromTable(rYa,rU);
      */

      pt[2]=getRfromTable(rYa,rV);
      pt[1]=getGfromTable(rYa,rU,rV);
      pt[0]=getBfromTable(rYa,rU);

      pt+=4; // das nächste Pixel beim Target

      /*  term1 = 1.164*(ps[2]-16);
      pt[2] = (char)( term1 + 1.596*(ps[3]-128) );//R
      pt[1] = (char)( term1 - 0.813*(ps[3]-128) - 0.391*(ps[1] - 128) );//G
      pt[0] = (char)( term1 + 2.018*(ps[1]-128) );//B
      pt[3] = alpha;
      */
      /*
      pt[2]=R_FROMYV(rYb,rV);
      pt[1]=G_FROMYUV(rYb,rU,rV);
      pt[0]=B_FROMYU(rYb,rU);
      */
      /*
      pt[2]=Rtable[rYb][rV];//getRfromTable(rYb,rV);
      pt[1]=Gtable[rYb][rU][rV];//getGfromTable(rYb,rU,rV);
      pt[0]=Btable[rYb][rU];//getBfromTable(rYb,rU);
      */
      /*
      pt[2]=Rtable[ps[2]][ps[3]];//getRfromTable(rYb,rV);
      pt[1]=Gtable[ps[2]][ps[3]][ps[1]];//getGfromTable(rYb,rU,rV);
      pt[0]=Btable[ps[2]][ps[1]];//getBfromTable(rYb,rU);
      */

      pt[2]=getRfromTable(rYb,rV);
      pt[1]=getGfromTable(rYb,rU,rV);
      pt[0]=getBfromTable(rYb,rU);

      pt+=4;
      ps+=4;
    }
}

static void processRGBImages(struct v4l_capture* cap,const void * p,int method,size_t len)
{
  static SDL_Surface * pSf = 0;
  static SDL_Surface * pSf1 = 0;
  static int initNotDone = 1;
  //printf("processRGBImages len:%i\n",len);
  if(method==IO_METHOD_MMAP)
    {
      if(initNotDone)
	{
#ifdef EUMAX01_MJPEG_SUPPORT
	  initLut();
#endif
	  printf("initRGBtables()...\n");
	  initRGBtables();
	  printf("initRGBtables() done!\n");
	  printf("mainSurface BitsPerPixel:%i, BytesPerPixel :%i\n",	\
		 cap->mainSurface->format->BitsPerPixel,		\
		 cap->mainSurface->format->BytesPerPixel);
	  //SDL_SWSURFACE|SDL_SRCALPHA,
	  pSf =SDL_CreateRGBSurface(SDL_HWSURFACE,			\
				    cap->camWidth,			\
				    cap->camHeight,			\
				    32,					\
				    SDL_GetVideoSurface()->format->Rmask, \
				    SDL_GetVideoSurface()->format->Gmask, \
				    SDL_GetVideoSurface()->format->Bmask, \
				    SDL_GetVideoSurface()->format->Amask);
	  if(!pSf)
	    {
	      printf("SDL_CreateRGBSurface failed in processRGBImages for cam 0\n");
	      exit(-1);
	    }
	  pSf1 =SDL_CreateRGBSurface(SDL_HWSURFACE,			\
				     cap->camWidth,			\
				     cap->camHeight,			\
				     32,				\
				     SDL_GetVideoSurface()->format->Rmask, \
				     SDL_GetVideoSurface()->format->Gmask, \
				     SDL_GetVideoSurface()->format->Bmask, \
				     SDL_GetVideoSurface()->format->Amask);
	  if(!pSf1)
	    {
	      printf("SDL_CreateRGBSurface failed in processRGBImages for cam 1\n");
	      exit(-1);
	    }
	  printf("pSf pitch:%i\n",pSf->pitch);
	  printf("SDL_GetVideoSurface()->format->Rmask: %x\n",SDL_GetVideoSurface()->format->Rmask);
	  printf("SDL_GetVideoSurface()->format->Gmask: %x\n",SDL_GetVideoSurface()->format->Gmask);
	  printf("SDL_GetVideoSurface()->format->Bmask: %x\n",SDL_GetVideoSurface()->format->Bmask);
	  printf("SDL_GetVideoSurface()->format->Amask: %x\n",SDL_GetVideoSurface()->format->Amask);
	  printf("SDL_GetVideoSurface()->format->BitsPerPixel: %i\n",SDL_GetVideoSurface()->format->BitsPerPixel);
	  printf("SDL_GetVideoSurface()->format->BytesPerPixel: %i\n",SDL_GetVideoSurface()->format->BytesPerPixel);
	  printf("SDL_GetVideoSurface()->format->alpha: %i\n",SDL_GetVideoSurface()->format->alpha);
	  //printf("SDL_GetVideoSurface()->format->palette: %i\n",SDL_GetVideoSurface()->format->palette);
	  initNotDone = 0;
	}
      else if(cap->camnumber==0)
	{
	  char * pc = (char *)p;
	  convertYUYVtoRGB(pc,(char *)pSf->pixels,cap->camWidth,cap->camHeight);
	  SDL_Rect destrect;
	  destrect.x = 10;
	  destrect.y = 10;
	  destrect.w=cap->camWidth;
	  destrect.h = cap->camHeight;
	  if(SDL_BlitSurface(pSf,0,cap->mainSurface,&destrect))
	    printf("Blitting failed in processRGBImages\n");
	  if(SDL_Flip(cap->mainSurface))
	    printf("Flipping failed in processRGBImages\n");
	}//end if cap->camnumber == 0
      else if(cap->camnumber==1)
	{
	  char * pc = (char *)p;
	  convertYUYVtoRGB(pc,(char *)pSf1->pixels,cap->camWidth,cap->camHeight);
	  SDL_Rect destrect;
	  destrect.x = 10+cap->camWidth;
	  destrect.y = 10;
	  destrect.w=cap->camWidth;
	  destrect.h = cap->camHeight;
	  if(SDL_BlitSurface(pSf1,0,cap->mainSurface,&destrect))
	    printf("Blitting failed in processRGBImages\n");
	  if(SDL_Flip(cap->mainSurface))
	    printf("Flipping failed in processRGBImages\n");	  
	}
      else
	{
	  printf("unknown camnumber: %i, should not occur!\n",cap->camnumber);
	}
    }
  else if(method==IO_METHOD_USERPTR)
    {
      printf("IO_METHOD_USEPTR not supported in process_image2\n");
    }
  else
    {
      fputc ('_', stdout);
      fflush (stdout);
    }
}


CamControl::CamControl(GUI * pGUI,int Pixelformat,bool RGB_Mode,int yBottomSide)
{
  this->ptheGUI = pGUI;
  this->RGB_Mode = RGB_Mode;
  this->yBottomSide = yBottomSide;

  if(Pixelformat)
    this->PixelFormat = 1;//MJPEG
  else
    this->PixelFormat = 0;//uncompressed

  cap_init(pGUI->getMainSurface(),		\
	   camwidth,				\
	   camheight,				\
	   sdlwidth,				\
	   sdlheight,				\
	   0,					\
	   this->PixelFormat);
  cam0ready=false;
  cam1ready=false;
  pPollTimer = new PollTimer(1000,this);
  pPollReaderCam0 = new PollReader(this);
  pPollReaderCam1 = new PollReader(this);

  pGUI->addPollTimer(pPollTimer);
  // cap_uninit();

#ifdef C6COMPILE
  this->framebuffer0 =
    (unsigned char *) C6RUN_MEM_calloc(1,
			     (size_t) camwidth * (camheight +
						   8) * 2);
  this->framebuffer1 =
    (unsigned char *) C6RUN_MEM_calloc(1,
			     (size_t) camwidth * (camheight +
						   8) * 2);
#else
  this->framebuffer0 =
    (unsigned char *) calloc(1,
			     (size_t) camwidth * (camheight +
						   8) * 2);
  this->framebuffer1 =
    (unsigned char *) calloc(1,
			     (size_t) camwidth * (camheight +
						   8) * 2);
#endif /* C6COMPILE */
}

void CamControl::pollReadEvent(PollSource * s)
{
  if(s==pPollReaderCam0)
    {
      //printf("0");
      if(-1==cap_read_frame(0))
	{
	  printf("error on polling camera0\n");
	}
    }
    if(s==pPollReaderCam1)
    {
      //printf("1");
      if(-1==cap_read_frame(1))
	{
	  printf("error on polling camera1\n");
	}
    }
}

void CamControl::pollTimerExpired(long us)
{
  bool again = false;
  int camfd = 0;
  if(!this->cam0ready)
    {
      if(this->PixelFormat)
	{
	  if(this->RGB_Mode)
	    camfd=cap_cam_init(0,processRGBImages);
	  else
	    camfd=cap_cam_init(0,processMJPEG);
	}
      else
	{
	  if(this->RGB_Mode)
	    camfd=cap_cam_init(0,processRGBImages);
	  else
	    camfd=cap_cam_init(0,processImages);
	}
      if(camfd<0)
	{
	  again = true;
	}
      else
	{
	  printf("add cam0 to PollManager\n");
	  this->cam0ready = true;
	  this->pPollReaderCam0->setReadSource(camfd,(char *)"cam0");
	  if(this->ptheGUI->addPollReader(pPollReaderCam0)!=0)
	    printf("addPollReader failed\n");
	  if(cap_cam_enable50HzFilter(camfd))
	    {
	      printf("enable50HzFilter failed\n");
	    }
	  cap_cam_setOverlayBottomSide(this->yBottomSide);
	  if(setV4L2_Value(camfd,V4L2_CID_BACKLIGHT_COMPENSATION,2))
	    {
	      printf("set V4L2_CID_BACKLIGHT_COMPENSATION 2 failed for cam0\n");
	    }
	}
    }

  if(!this->cam1ready)
    {
      if(this->PixelFormat)
	{
	  if(this->RGB_Mode)
	    camfd=cap_cam_init(1,processRGBImages);
	  else
	    camfd=cap_cam_init(1,processMJPEG);
	}
      else
	{
	  if(this->RGB_Mode)
	    camfd=cap_cam_init(1,processRGBImages);
	  else
	    camfd=cap_cam_init(1,processImages);
	  }
      if(camfd<0)
	{
	  again = true;
	}
      else
	{
	  printf("add cam1 to PollManager\n");
	  this->cam1ready = true;
	  this->pPollReaderCam1->setReadSource(camfd,(char*)"cam1");
	  if(this->ptheGUI->addPollReader(pPollReaderCam1)!=0)
	    printf("addPollReader failed\n");
	  if(cap_cam_enable50HzFilter(camfd))
	    {
	      printf("enable50HzFilter failed\n");
	    }
	  cap_cam_setOverlayBottomSide(this->yBottomSide);
	  if(setV4L2_Value(camfd,V4L2_CID_BACKLIGHT_COMPENSATION,2))
	    {
	      printf("set V4L2_CID_BACKLIGHT_COMPENSATION 2 failed for cam1\n");
	    }
	}
    }
  if(again)
    {
      //printf("try to find camera again...\n");
    }
  else
    {
      //printf("TODO remove PollTimer\n");
    }
  oneSecondTimer();
}

/*
static void showVideoMode()
{
  //  typedef struct{
  //  Uint32 hw_available:1;
  //  Uint32 wm_available:1;
  //  Uint32 blit_hw:1;
  //  Uint32 blit_hw_CC:1;
  //  Uint32 blit_hw_A:1;
  //  Uint32 blit_sw:1;
  //  Uint32 blit_sw_CC:1;
  //  Uint32 blit_sw_A:1;
  //  Uint32 blit_fill;
  //  Uint32 video_mem;
  //  SDL_PixelFormat *vfmt;
  //} SDL_VideoInfo;

//Structure Data

//hw_available	Is it possible to create hardware surfaces?
//wm_available	Is there a window manager available
//blit_hw	Are hardware to hardware blits accelerated?
//blit_hw_CC	Are hardware to hardware colorkey blits accelerated?
//blit_hw_A	Are hardware to hardware alpha blits accelerated?
//blit_sw	Are software to hardware blits accelerated?
//blit_sw_CC	Are software to hardware colorkey blits accelerated?
//blit_sw_A	Are software to hardware alpha blits accelerated?
//blit_fill	Are color fills accelerated?
//video_mem	Total amount of video memory in Kilobytes
//vfmt	Pixel format of the video device

//palette	Pointer to the palette, or NULL if the BitsPerPixel>8
//BitsPerPixel	The number of bits used to represent each pixel in a surface. Usually 8, 16, 24 or 32.
//BytesPerPixel	The number of bytes used to represent each pixel in a surface. Usually one to four.
//[RGBA]mask	Binary mask used to retrieve individual color values
//[RGBA]loss	Precision loss of each color component (2[RGBA]loss)
//[RGBA]shift	Binary left shift of each color component in the pixel value
//colorkey	Pixel value of transparent pixels
//alpha	Overall surface alpha value

  const SDL_VideoInfo * info;
  //SDL_PixelFormat * pixFormat;
  info = SDL_GetVideoInfo();
  //pixFormat = info->vfmt;
  if(info)
    {
      if(info->vfmt)
	{
	  printf("SDL_GetVideoFormat Pixelformat  BPP:%i \n",info->vfmt->BitsPerPixel);
	}
    }
    }*/

const char * usage =				\
  "cap -xga for 1024x768 else PAL Widescreen with 1024*576\n"\
  "    -fullscreen for Fullscreen\n"\
  "    -m for MJPEG (normally use RAW\n";

static void onExit(int i,void* pv)
{
  printf("Version_A: %s\n",CAP_VERSION);
  printf("Version_B: %s\n",FSGPP_VERSION);
  printf("Version_C: %s\n",CAPTURE_VERSION);
  printf("Programm Version: %s\n",CAPCOMPILEDATE);
}

ArbeitsDialog * theArbeitsDialog;
MBProtocol theProtocol;
Rezept theRezept;

  GUI* theGUI;
  char tmp[64];
  bool Com_NON_BLOCK = false;
  bool guiMode = false;

static void oneSecondTimer(void)
{
  static bool serialCommClosed = true;
  static int SplashScreenTimer = 0;

  if(serialCommClosed)
    {
      if(theProtocol.initProtocol(theGUI,theArbeitsDialog,tmp,Com_NON_BLOCK))
	{
	  printf("Uart communication failed, trying again in one second\n");
	}
      else
	{
	  serialCommClosed = false;

	  theArbeitsDialog->sendProtocolMsg(nPEC_SETQMAX1,MBProtocol::QMAX);
	  theArbeitsDialog->sendProtocolMsg(nPEC_SETQMAX2,MBProtocol::QMAX);
	  theArbeitsDialog->sendProtocolMsg(nPEC_SET_FP1,theProtocol.getLastPositionFP1());
	  theArbeitsDialog->sendProtocolMsg(nPEC_SET_FP2,theProtocol.getLastPositionFP2());
	  if(theProtocol.enableAuto())
	    {
	      printf("Protocoll enableAuto failed\n");
	    }
	  theArbeitsDialog->sendProtocolMsg(nPEC_SWVERSION);
	  theArbeitsDialog->sendProtocolMsg(nPEC_HWVERSION);
	  theArbeitsDialog->sendProtocolMsg(nPEC_GET_Q1);//für den CalDialog
	  theArbeitsDialog->sendProtocolMsg(nPEC_LIGHTON);
	}
    }

  if(SplashScreenTimer<5)
    {
      SplashScreenTimer++;
    }
  else if(SplashScreenTimer==5)
    {
      SplashScreenTimer++;
      if(guiMode &&							\
	 theArbeitsDialog->theActiveDialogNumber()==ArbeitsDialog::SplashScreenIsActive)
	{
	  theArbeitsDialog->showCalibrationDialog();
	}
    }

}

int main(int argc, char *argv[])
{
  //SDL_version compile_version;

  GUI_Properties props;
  int Pixelformat = 0;//0 = normal, 1 = MJPEG, 2 = RGB
  bool rgb_mode = false;
  int ButtonAreaHeight = 0;

  char path[64];
  char confpath[96];
  char saveFilePath[96];
  bool FullScreenMode = false;
  bool HideMouseCursor = false;

  FILE *fp;

  if(Tool::getAppPath(argv[0],path,64))
    {
      perror("can`t get application path\n");
    }
  if(snprintf(confpath,96,"%s../cap.conf",path)<0)
    {
      perror("can`t create config file path\n");
    }
  
  fp = fopen(confpath,"r");
  if(fp)
    {
      // exists
      fclose(fp);
    } 
  else 
    {
      // doesnt exist
      printf("\n");
      printf("Cannot find config file : %s\n",confpath);
      printf("The config file cap.conf is needed to run and could not be found!\n");
      printf("If this is a new installation try to use the BEISPIEL.cap.conf\n");
      printf("\n");
      printf("cp BEISPIEL.cap.conf ../cap.conf\n");
      printf("\n");
      printf("... edit cap.conf and try again.\n");
      return -1;
    }

  printf("Version_A: %s\n",CAP_VERSION);
  printf("Version_B: %s\n",FSGPP_VERSION);
  printf("Version_C: %s\n",CAPTURE_VERSION);
  printf("Programm Version: %s\n",CAPCOMPILEDATE);
  on_exit(onExit,0);

  props.width=0;
  props.height=0;
  props.bpp=0;
  props.flags=0;

  if(!iniParser_getParam(confpath,(char*)"sdlwidth",tmp,64))
    {
      sdlwidth = atoi(tmp);
      printf("sdlwidth = %i\n",sdlwidth);
    }
  if(!iniParser_getParam(confpath,(char*)"sdlheight",tmp,64))
    {
      sdlheight = atoi(tmp);
      printf("sdlheigth = %i\n",sdlheight);
    }
  if(!iniParser_getParam(confpath,(char*)"camwidth",tmp,64))
    {
      camwidth = atoi(tmp);
      printf("camwidth = %i\n",camwidth);
    }
  if(!iniParser_getParam(confpath,(char*)"camheight",tmp,64))
    {
      camheight = atoi(tmp);
      printf("camheight = %i\n",camheight);
    }
  if(!iniParser_getParam(confpath,(char*)"PathSaveFile",saveFilePath,96))
    {
      printf("saveFilePath = %s\n",saveFilePath);
    }
  else
    {
      saveFilePath[0] ='d';
      saveFilePath[1] ='a';
      saveFilePath[2] ='t';
      saveFilePath[3] ='a';
      saveFilePath[4] ='\0';
      printf("reading saveFilePath failed, using default path %s\n",saveFilePath); 
    }
  if(!iniParser_getParam(confpath,(char*)"pixelformat",tmp,64))
    {
      if(!strcmp("mjpeg",tmp))
	{
	  Pixelformat=1;
	  printf("pixelformat = MJPEG\n");
	}
      else
	{
	  Pixelformat = 0;
	  printf("pixelformat = NORMAL\n");
	}	  
    }
  if(!iniParser_getParam(confpath,(char*)"rgb_mode",tmp,64))
    {
      if(!strcmp("true",tmp))
	{
	  rgb_mode=true;
	  printf("using RGB_MODE\n");
	}
    }
  if(!iniParser_getParam(confpath,(char*)"GUIMode",tmp,64))
    {
      guiMode = true;
    }
  if(!iniParser_getParam(confpath,(char*)"Fullscreen",tmp,64))
    {
      props.flags|=SDL_FULLSCREEN;
      FullScreenMode = true;
    }
   if(!iniParser_getParam(confpath,(char*)"Com_NON_BLOCK",tmp,64))
    {
      Com_NON_BLOCK = true;
    }
   if(!iniParser_getParam(confpath,(char*)"HIDE_MOUSE_CURSOR",tmp,64))
    {
      HideMouseCursor=true;
    }
  //das Muss der letzte Paramter sein der mit tmp geholt wird, da tmp 
  //später noch ausgewertet wird!
  if(iniParser_getParam(confpath,(char*)"usbDevice",tmp,64))
    {//"/dev/ttyACM0" ist Standart
      tmp[0]='/'; 
      tmp[1]='d';
      tmp[2]='e';
      tmp[3]='v';
      tmp[4]='/';
      tmp[5]='t';
      tmp[6]='t';
      tmp[7]='y';
      tmp[8]='A';
      tmp[9]='C';
      tmp[10]='M';
      tmp[11]='0';
      tmp[12]='\0';
    }


  argc--;
  while(argc)
    {
      if(!strcmp(argv[argc],"-xga")||!strcmp(argv[argc],"-XGA"))
	{
	  sdlwidth = 1024;
	  sdlheight = 768;
	}
      else if(!strcmp(argv[argc],"-fullscreen"))
	{
	  props.flags|=SDL_FULLSCREEN;
	}
      else if(!strcmp(argv[argc],"-m"))
	{
	  Pixelformat=1;
	}
      else
	{
	  printf("%s",usage);
	  return 0;
	}
      argc--;
    }

  props.width=sdlwidth;//1280;//720;
  props.height=sdlheight;//576;
  props.bpp=32;
  //props.flags|=SDL_SWSURFACE;//SDL_HWSURFACE;//|SDL_DOUBLEBUF;
  props.flags|=SDL_ANYFORMAT;

  if(SDL_BYTEORDER==SDL_BIG_ENDIAN)
    {
      printf("SDL_BYTEORDER==SDL_BIG_ENDIAN\n");
    }
  if(SDL_BYTEORDER==SDL_LIL_ENDIAN)
    {
      printf("SDL_BYTEORDER==SDL_LIL_ENDIAN\n");
    }

  theGUI=GUI::getInstance(&props,/*theSecondaryEvtHandling*/0,FullScreenMode);
  if(!theGUI){
    printf("GUI::getInstance failed\n");
    return -1;
  }
  if(HideMouseCursor)
    {
      SDL_ShowCursor(SDL_DISABLE);
    }
  /*  showVideoMode();
  int suggestedbbp = SDL_VideoModeOK(props.width, props.height, props.bpp,props.flags);
  if(suggestedbbp)
    {
      printf("SDL_VideoModeOK says: OK!, bbp=%i\n",suggestedbbp);
    }
  else
    {
      printf("SDL_VideoModeOK says: Mode not available.\n");
      printf("Couldn't initialize SDL: %s\n", SDL_GetError());
      exit(-1);
      }*/

  theProtocol = MBProtocol();

  //ButtonAreaHeight = props.height - 168;
  ButtonAreaHeight = camheight;
  camCtrl = new CamControl(theGUI,Pixelformat,rgb_mode,ButtonAreaHeight);

  theArbeitsDialog = new ArbeitsDialog(theGUI,			\
				       &theProtocol,		\
				       props.width,		\
				       props.height,		\
				       camwidth,		\
				       camheight,		\
				       ButtonAreaHeight,	\
				       saveFilePath,\
				       guiMode);


  theGUI->eventLoop();
}
