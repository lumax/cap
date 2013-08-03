/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __MBPROT_H__
#define __MBPROT_H__

  class PollReader;
  class PollTimer;
  class IMBProtListener;

  class MBProtocol:IPollTimerListener,IPollReadListener
  {
  public:
    MBProtocol();
    
    virtual void pollReadEvent(PollSource * s);
    virtual void pollTimerExpired(long us);
    int initProtocol(GUI * pGUI,\
		     IMBProtListener * listener,\
		     char * device,\
		     bool non_block);
    IMBProtListener * lis;
    void closeProtocol();
    int getQ1();
    int enableAuto();
    static int disableAuto();
    bool isInitialised();
    void getLastPositionsFromFile(int sdlw,int camw);
    void saveLastPositionsToFile();
    int getLastPositionFP1();
    int getLastPositionFP2();
    void setLastPositionFP1(unsigned short dat);
    void setLastPositionFP2(unsigned short dat);

    int verbose;
  private:
    int lastPositionFP1;
    int lastPositionFP2;
    bool isInit;
    struct termios termOptions;
    int fd;
    PollTimer * pPollTimer;
    PollReader * pPollIncoming;
    //void dispatcher(unsigned char ucDat);
  };
  
#endif /* __MBPROT_H__*/
}//end namespace
