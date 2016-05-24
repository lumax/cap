//#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>
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
#include "videodev.h"
#include "V4L2_utils.h"
#include "iniParser.h"

#include "g_ctrl/G_Ctrl.h"
#include "g_ctrl/StreamScanner.h"
#include "g_ctrl/ExaktG.h"

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
  bool cam0ready;
  bool cam1ready;
private:
  int yBottomSide;
  bool RGB_Mode;
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
static int RectBreite = 0;
static int RectHoehe = 0;

static const char * pathFadenkreuzBreite = "../CrossairWidth.conf";
static  int CrossBreite = 0;

static const unsigned int MAXCAMWIDTH = 1280;
static unsigned char oneLineColor[MAXCAMWIDTH*2];
static int circleQuarterX[MAXCAMWIDTH/2];
static int OneCam = 0;

//static double FaktorZ1 = 1.0;

static CamControl * camCtrl;

static void setFadenkreuzBreite(int val)
{
  CrossBreite = val;
  if(CrossBreite < 0)
    CrossBreite = 0;
  if(CrossBreite > 14)
    CrossBreite = 14;
}

static int getFadenkreuzBreite(void)
{
  return CrossBreite;
}

static void saveFadenkreuzBreite(void)
{
  FILE * fd;
  fd = fopen(pathFadenkreuzBreite, "w");//RCap_max in Datei schreiben

  if(fd)
    fprintf(fd,"%i",getFadenkreuzBreite());

  fclose(fd);
}

static int loadFadenkreuzBreite(void)
{
  FILE * fd;
  char tmp[64];
  char * s;

  fd = fopen(pathFadenkreuzBreite,"r");
  if(fd)
    {
      if((s = fgets (tmp, sizeof(tmp), fd)) != NULL)
	{
	  setFadenkreuzBreite(atoi(s));
	}
      fclose(fd);
      return getFadenkreuzBreite();
    }
  else
    {
      return 0;
    }
}

static int getOverlayCircle(void)
{
  return RectBreite;
}

static void prepareOverlayCircle(int newDiameter)
{
  int max = 0;
  if((unsigned int)camheight > MAXCAMWIDTH/2-20)
    max = MAXCAMWIDTH/2;
  else
    max = camheight;

  RectBreite = newDiameter;
  if(RectBreite<0)
    RectBreite*=-1;
  if(RectBreite>(max))
    RectBreite = (max);
  RectHoehe = RectBreite;

  for(int i=0;i<=RectHoehe/2;i++)
    {
      /*
	sin(alpha) = Yp / a ; wobei a die Hypotenuse ist!
	cos(alpha) = Xp / a

	sin(alpha) = +- Wurzel(1 - (cos(alpha)^2 )
	ergibt:
	Yp / a = +- Wurzel(1 - (Xp / a)^2 )
	nach x aufgelöst ergibt in etwa:
	x = Wurzel( a^2 - y^2 )
	...somit hat man einen viertel Kreis!
      */
      circleQuarterX[i] = sqrt(pow(RectHoehe/2,2)-pow(i,2));
    }
}

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

/*
  Es gibt zwei Koordinatensysteme:
  1. das Normale x und y:     2. das USB-Image

  0------>x                   0-------->w = 2*w bytes
  |                           |
  |                           |
  |                           |
  Y                        h = 2*w bytes * h

  w = x*2
  h = y*w*2

 */
static inline void zeichneZeile(int w,	\
				int h,	\
				int x,	\
				int y,	\
				int breite,	\
				 char * pc)
{
  static unsigned int MaxValueInPC = 0;
  unsigned int len = 0;
  unsigned int start = 0;

  if(x<0)
    x=0;
  if(y<0)
    y=0;
  if(breite<0)
    breite=0;

  //start = h + w = y*w*2 + x*2
    start = y*w*2 + x*2;

  //Max Byte in pc = w*h*2
  if(!MaxValueInPC)
    MaxValueInPC = h*w*2;

  if(breite<0)
    breite = 0;
   len = breite*2;

  if((len+start)>=MaxValueInPC)
    len = MaxValueInPC-1;

  memcpy(&pc[start],oneLineColor,breite*2);

}

static inline void zeichneSpalte(int w,	\
				 int h,	\
				 int x,	\
				 int y,	\
				 int hoehe,	\
				 char * pc)
{
  static unsigned int MaxValueInPC = 0;
  unsigned int start = 0;

  if(x<0)
    x=0;
  if(y<0)
    y=0;
  if(hoehe<0)
    hoehe=0;

  //Max Byte in pc = w*h*2
  if(!MaxValueInPC)
    MaxValueInPC = h*w*2;

  //start = h + w = y*w*2 + x*2
  start = y*w*2 + x*2;

  for(int ii=0;ii<hoehe;ii++)
    {
      for(int i=-CrossBreite/2;i<=(CrossBreite/2);i++){
	if(i==-CrossBreite/2||i==CrossBreite/2)
	  memcpy(&pc[start+i*4],oneLineColor,4);
      }
      start+=w*2;
      if(start>=MaxValueInPC)
	return;
    }
}

static void overlayAndRect(struct v4l_capture* cap,char * pc,size_t len)
{
  unsigned int i;
  unsigned int w = (unsigned int)cap->camWidth;
  unsigned int h = (unsigned int)cap->camHeight;
  int alles = 0;
  int cam = cap->camnumber;
  int wMalZwei = w*2;
  int wMalVier = w*4;
  int offset = cam*wMalZwei;

  unsigned int crossX = cap->camCrossX;

  static int flag = 1;
  static unsigned char Y = 106,U = 221,V = 202;


    if(flag)
    {
      Y = getYfromRGB(0,255,0);//gruen
      U = getUfromRGB(0,255,0);
      V = getVfromRGB(0,255,0);
      printf("Y = %i, U=%i, V=%i\n",Y,U,V);
      flag=0;
      }

/*
       vMitteOben: x = crossX, y = 0; hoehe = h/2 - RectHoehe/2
         |
         |
    ------------hRectOben: y=(h/2 - RectHoehe/2), x = crossX-RectBreite/2, Breite = RectBreite

    vRectLinks: x = crossX-RectBreite/2,y = h/2 - RectHoehe/2, hoehe = RectHoehe
    |          |
    |          |
    |          |
    |          |
               vRectRechts: x = crossX+RectBreite/2,y = h/2 - RectBreite/2,hoehe = RectHoehe+1

    ------------hRectUnten: x = crossX-RectBreite/2,y=(h/2 + RectHoehe/2),Breite = RectBreite
         |
         |
         |vMitteUnten: x = crossX, y = h/2+RectHoehe/2; hoehe = h/2 - RectHoehe/2

---- hMitteLinks: x = 0,y = h/2, Breite = crossX - RectBreite/2

                ------
		hMitteRechts: x=crossX+RectBreite/2, y=h/2, Breite=w-(crossX + RectBreite/2)

*/
    zeichneSpalte(w,h,crossX,0,h/2-RectHoehe/2,pc);//vMitteOben
    zeichneSpalte(w,h,crossX,h/2+RectHoehe/2,h/2-RectHoehe/2,pc);//vMitteUnten
    zeichneSpalte(w,h,crossX-RectBreite/2,h/2-RectHoehe/2,RectHoehe,pc);//vRectLinks
    zeichneSpalte(w,h,crossX+RectBreite/2,h/2-RectHoehe/2,RectHoehe+1,pc);//vRectRechts
    zeichneZeile(w,h,crossX-RectBreite/2,h/2-RectHoehe/2,RectBreite,pc);//hRectOben
    zeichneZeile(w,h,crossX-RectBreite/2,h/2+RectHoehe/2,RectBreite,pc);//hRectUnten
    zeichneZeile(w,h,0,h/2,crossX-RectBreite/2,pc);//hMitteLinks
    zeichneZeile(w,h,crossX+RectBreite/2,h/2,w-(crossX+RectBreite/2),pc);//hMitteRechts

    //auf Overlay kopieren
  for(i=0;i<h;i++)
    {
      memcpy(cap->sdlOverlay->pixels[0]+i*wMalVier+offset,	\
	     pc+alles,						\
	     wMalZwei);
	  alles += w*2;
    }
}

static void overlayAndCircle(struct v4l_capture* cap,char * pc,size_t len)
{
  unsigned int i;
  unsigned int w = (unsigned int)cap->camWidth;
  unsigned int h = (unsigned int)cap->camHeight;
  int alles = 0;
  int cam = cap->camnumber;
  int wMalZwei = w*2;
  int wMalVier = w*4;
  int offset = cam*wMalZwei;

  unsigned int crossX = cap->camCrossX;

  static int flag = 1;
  static unsigned char Y = 106,U = 221,V = 202;


    if(flag)
    {
      Y = getYfromRGB(0,255,0);//gruen
      U = getUfromRGB(0,255,0);
      V = getVfromRGB(0,255,0);
      printf("Y = %i, U=%i, V=%i\n",Y,U,V);
      flag=0;
    }


    for(int i=0;i<=RectHoehe/2;i++)//Kreis
      {
	zeichneZeile(w,h,crossX+circleQuarterX[i],i+h/2,2,pc);//rechts mitte nach unten
	zeichneZeile(w,h,crossX+circleQuarterX[i],h/2-i,2,pc);//rechts mitte nach oben

	zeichneZeile(w,h,crossX-circleQuarterX[i],i+h/2,2,pc);
	zeichneZeile(w,h,crossX-circleQuarterX[i],h/2-i,2,pc);// links mitte nach oben
      }
    zeichneSpalte(w,h,crossX,0,h,pc);//Vertikal

    for(int i=-CrossBreite;i<=CrossBreite;i++)
      {
	if(i==-CrossBreite||i==CrossBreite)
	  zeichneZeile(w,h,0,(h/2)+i,w,pc);//horizontal
      }

    //auf Overlay kopieren
    for(i=0;i<h;i++)
      {
	memcpy(cap->sdlOverlay->pixels[0]+i*wMalVier+offset,	\
	       pc+alles,					\
	       wMalZwei);
	alles += w*2;
      }
}

static void overlayAndCircleOneCam(struct v4l_capture* cap,char * pc,size_t len)
{
  unsigned int i;
  unsigned int w = (unsigned int)cap->camWidth;
  unsigned int h = (unsigned int)cap->camHeight;
  int alles = 0;
  int cam = cap->camnumber;
  int wMalZwei = w*2;
  int wMalVier = w*4;
  int offset = 0;//w;//w+2 ist die Mitte, w ist das erste viertelcam*wMalZwei;

  unsigned int crossX = cap->camCrossX;

  static int flag = 1;
  static unsigned char Y = 106,U = 221,V = 202;

  if(0!=cam)//OneCam
    return;

    if(flag)
    {
      Y = getYfromRGB(0,255,0);//gruen
      U = getUfromRGB(0,255,0);
      V = getVfromRGB(0,255,0);
      printf("Y = %i, U=%i, V=%i\n",Y,U,V);
      flag=0;
    }


    for(int i=0;i<=RectHoehe/2;i++)
      {
	zeichneZeile(w,h,crossX+circleQuarterX[i],i+h/2,2,pc);//rechts mitte nach unten
	zeichneZeile(w,h,crossX+circleQuarterX[i],h/2-i,2,pc);//rechts mitte nach oben

	zeichneZeile(w,h,crossX-circleQuarterX[i],i+h/2,2,pc);
	zeichneZeile(w,h,crossX-circleQuarterX[i],h/2-i,2,pc);// links mitte nach oben

      }

    zeichneSpalte(w,h,crossX,0,h/2-RectHoehe/2,pc);//vMitteOben
    zeichneSpalte(w,h,crossX,h/2+RectHoehe/2,h/2-RectHoehe/2,pc);//vMitteUnten
    //zeichneSpalte(w,h,crossX-RectBreite/2,h/2-RectHoehe/2,RectHoehe,pc);//vRectLinks
    //zeichneSpalte(w,h,crossX+RectBreite/2,h/2-RectHoehe/2,RectHoehe+1,pc);//vRectRechts
    //zeichneZeile(w,h,crossX-RectBreite/2,h/2-RectHoehe/2,RectBreite,pc);//hRectOben
    //zeichneZeile(w,h,crossX-RectBreite/2,h/2+RectHoehe/2,RectBreite,pc);//hRectUnten
    zeichneZeile(w,h,0,h/2,crossX-RectBreite/2,pc);//hMitteLinks
    zeichneZeile(w,h,crossX+RectBreite/2,h/2,w-(crossX+RectBreite/2),pc);//hMitteRechts

    //auf Overlay kopieren
  for(i=0;i<h;i++)
    {
      memcpy(cap->sdlOverlay->pixels[0]+i*wMalVier+offset,	\
	     pc+alles,						\
	     wMalZwei);
	  alles += w*2;
    }
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

      if(0)//Verhindert Warnung bei nichtbenutzter Funktion
	overlayAndRect(cap,(char *)framebuffer,len);

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

      if(1==OneCam)
	overlayAndCircleOneCam(cap,(char *)pc,len);
      else
	overlayAndCircle(cap,(char *)pc,len);

      SDL_UnlockYUVOverlay(cap->sdlOverlay);
      SDL_UnlockSurface(cap->mainSurface);
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


ArbeitsDialog * theArbeitsDialog;
MBProtocol theProtocol;
Rezept theRezept;
ExaktG * pExaktG = 0;

GUI* theGUI;
char tmp[64];
char pathTinyG[64];
bool Com_NON_BLOCK = false;
bool guiMode = false;
static bool serialCommClosed = true;
static bool IGNORE_MISSING_TERMINAL = false;
static bool lightIsOn = false;
static float ExaktGSAbstand = ExaktG::defaultGSicherheitsabstand;
static float ExaktGMaxS = ExaktG::defaultGMaxStrecke;

void CamControl::pollTimerExpired(long us)
{
  bool again = false;
  int camfd = 0;
  if(!this->cam0ready&&(lightIsOn||IGNORE_MISSING_TERMINAL))
    {
      if(this->PixelFormat)
	{
	  if(this->RGB_Mode)
	    camfd=cap_cam_init(0,RectBreite/2,OneCam,processRGBImages);
	  else
	    camfd=cap_cam_init(0,RectBreite/2,OneCam,processMJPEG);
	}
      else
	{
	  if(this->RGB_Mode)
	    camfd=cap_cam_init(0,RectBreite/2,OneCam,processRGBImages);
	  else
	    camfd=cap_cam_init(0,RectBreite/2,OneCam,processImages);
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
	  theArbeitsDialog->FP1_evt((unsigned short)theProtocol.getLastPositionFP1());
	}
    }

  if(!this->cam1ready&&(lightIsOn||IGNORE_MISSING_TERMINAL))
    {
      if(this->PixelFormat)
	{
	  if(this->RGB_Mode)
	    camfd=cap_cam_init(1,RectBreite/2,OneCam,processRGBImages);
	  else
	    camfd=cap_cam_init(1,RectBreite/2,OneCam,processMJPEG);
	}
      else
	{
	  if(this->RGB_Mode)
	    camfd=cap_cam_init(1,RectBreite/2,OneCam,processRGBImages);
	  else
	    camfd=cap_cam_init(1,RectBreite/2,OneCam,processImages);
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
	  theArbeitsDialog->FP2_evt((unsigned short)theProtocol.getLastPositionFP2());
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

static char confpath[96] = "";
static char saveFilePath[96] = "";

const char * usage =				\
  "cap -xga for 1024x768 else PAL Widescreen with 1024*576\n"\
  "    -fullscreen for Fullscreen\n"\
  "    -m for MJPEG (normally use RAW\n";

static void onExit(int i,void* pv)
{
  int fd;
  if(camCtrl->cam0ready)
    {
      fd=cap_cam_getFd(0);
      close(fd);
    }
  if(camCtrl->cam1ready)
    {
      fd=cap_cam_getFd(1);
      close(fd);
    }
  if(0!=pExaktG)
    {
      pExaktG->getG_Ctrl()->closeUart();
    }
  printf("Version_A: %s\n",CAP_VERSION);
  printf("Version_B: %s\n",FSGPP_VERSION);
  printf("Version_C: %s\n",CAPTURE_VERSION);
  printf("Programm Version: %s\n",CAPCOMPILEDATE);
  printf("Config File Path: %s\n",confpath);
  printf("save File Path: %s\n",saveFilePath);
}

static void oneSecondTimer(void)
{
  static int SplashScreenTimer = 0;
  static int LightIsOnCounter = 0;
  static int ExaktG_com = 0;

  if(serialCommClosed)
    {
      if(theProtocol.initProtocol(theGUI,theArbeitsDialog,tmp,Com_NON_BLOCK))
	{
	  //printf("Uart communication failed, trying again in one second\n");
	}
      else
	{
	  serialCommClosed = false;

	  theArbeitsDialog->sendProtocolMsg(nPEC_SETQMAX1,		\
					    theArbeitsDialog->getCamW_Sichtbar());
	  theArbeitsDialog->sendProtocolMsg(nPEC_SETQMAX2,		\
					    theArbeitsDialog->getCamW_Sichtbar());
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
	  theArbeitsDialog->sendProtocolMsg(nPEC_NAME);
	}
    }
  else
    {
      if(!lightIsOn)
	{
	  LightIsOnCounter++;
	  if(LightIsOnCounter>=2)
	    {
	      lightIsOn = true;
	    }
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

  if(0==ExaktG_com)
    {
      printf("searching for ExaktG Komm Port\n");

      int ret = pExaktG->getG_Ctrl()->openUart(pathTinyG,115200);
      if(ret>0)
	{
	  pExaktG->setFD(ret);
	  theGUI->addPollTimer(pExaktG->getPollTimer());
	  if(0!=theGUI->addPollReader(pExaktG->getPollReader()))
	    {
	      printf("ExaktG addPollReader failed\n");
	    }
	  pExaktG->getG_Ctrl()->cmdFlowControl();
	  ExaktG_com = 1;
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
    }
  if(!iniParser_getParam(confpath,(char*)"sdlheight",tmp,64))
    {
      sdlheight = atoi(tmp);
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
  /*  if(!iniParser_getParam(confpath,(char*)"PathSaveFile",saveFilePath,96))
    {
      printf("saveFilePath = %s\n",saveFilePath);
    }
  else
  {*/
      saveFilePath[0] ='/';
      saveFilePath[1] ='o';
      saveFilePath[2] ='p';
      saveFilePath[3] ='t';
      saveFilePath[4] ='/';
      saveFilePath[5] ='c';
      saveFilePath[6] ='a';
      saveFilePath[7] ='p';
      saveFilePath[8] ='m';
      saveFilePath[9] ='b';
      saveFilePath[10] ='_';
      saveFilePath[11] ='d';
      saveFilePath[12] ='a';
      saveFilePath[13] ='t';
      saveFilePath[14] ='a';
      saveFilePath[15] ='/';
      saveFilePath[16] ='\0';
      printf("using default path %s\n",saveFilePath);
      /*printf("reading saveFilePath failed, using default path %s\n",saveFilePath);
        }*/
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
   if(!iniParser_getParam(confpath,(char*)"IGNORE_MISSING_TERMINAL",tmp,64))
    {
      IGNORE_MISSING_TERMINAL = true;
    }
   if(!iniParser_getParam(confpath,(char*)"HIDE_MOUSE_CURSOR",tmp,64))
    {
      HideMouseCursor=true;
    }
   /*  if(!iniParser_getParam(confpath,(char*)"FAKTOR_Z1",tmp,64))
    {
      FaktorZ1 = atof(tmp);
      printf("FAKTOR_Z1 = %f\n",(float)FaktorZ1);
      }*/
  if(!iniParser_getParam(confpath,(char*)"DIAMETER",tmp,64))
    {
      RectBreite = atoi(tmp);//wird in der Funktion prepareOverlayCircle Untersucht und Begrenzt
    }
  if(!iniParser_getParam(confpath,(char*)"ONECAM",tmp,64))
    {
      OneCam=1;
    }
  if(!iniParser_getParam(confpath,(char*)"G_SaveDistance",tmp,64))
    {
      ExaktGSAbstand = atof(tmp);
    }
  if(!iniParser_getParam(confpath,(char*)"G_MaxDistance",tmp,64))
    {
      ExaktGMaxS = atof(tmp);
    }
  if(iniParser_getParam(confpath,(char*)"TinyG_USB",pathTinyG,64))
    {
      pathTinyG[0]='/';
      pathTinyG[1]='d';
      pathTinyG[2]='e';
      pathTinyG[3]='v';
      pathTinyG[4]='/';
      pathTinyG[5]='t';
      pathTinyG[6]='t';
      pathTinyG[7]='y';
      pathTinyG[8]='U';
      pathTinyG[9]='S';
      pathTinyG[10]='B';
      pathTinyG[11]='1';
      pathTinyG[12]='\0';
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

  CrossBreite = loadFadenkreuzBreite();

  //Vorbereitungne für Circle (oder Rect)
  //
  unsigned char Y = 106,U = 221,V = 202;
  Y = getYfromRGB(0,255,0);//gruen
  U = getUfromRGB(0,255,0);
  V = getVfromRGB(0,255,0);
  for (unsigned int i=0;i<(MAXCAMWIDTH*4);i+=4)
    {
	      oneLineColor[i]=Y;
	      oneLineColor[i+1]=V;
	      oneLineColor[i+2]=Y;
	      oneLineColor[i+3]=U;
    }

  prepareOverlayCircle(RectBreite);
  //Vorbereitungen Ende

  props.width=sdlwidth;//1280;//720;
  props.height=sdlheight;//576;
  props.bpp=32;
  props.flags|=SDL_SWSURFACE;//SDL_HWSURFACE;//|SDL_DOUBLEBUF;
  //props.flags|=SDL_ANYFORMAT|SDL_HWSURFACE;

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

  pExaktG = new ExaktG(1,1,ExaktGSAbstand,ExaktGMaxS);//verboseExakt, verboseG
  if(0==pExaktG)
    {
      printf("create ExaktG instance failed\n");
      return -1;
    }

  /* GUI::getInstance verändert props.height und props.width im FullScreenMode.
   * In der GUI-Klasse heißt das UseCurWandH.
   * Diese mögliche Änderung wird nun für CamControl gesichert.*/
  sdlheight = props.height;
  sdlwidth = props.width;
  printf("sdlwidth = %i\n",sdlwidth);
  printf("sdlheigth = %i\n",sdlheight);

  if(HideMouseCursor)
    {
      SDL_ShowCursor(SDL_DISABLE);
    }

  theProtocol = MBProtocol();
  theProtocol.getLastPositionsFromFile(props.width,camwidth);

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
				       guiMode,\
				       prepareOverlayCircle,\
				       getOverlayCircle,\
				       setFadenkreuzBreite,\
				       saveFadenkreuzBreite,\
				       getFadenkreuzBreite,\
				       pExaktG);
  //theArbeitsDialog->setFaktorZAchse((float)FaktorZ1);

  theGUI->eventLoop();
}
