/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __MBPROT_H__
#define __MBPROT_H__

  class PollReader;
  class PollTimer;

  class MBProtocol:IPollTimerListener,IPollReadListener
  {
  public:
    MBProtocol();
    
    virtual void pollReadEvent(PollSource * s);
    virtual void pollTimerExpired(long us);
    int initProtocol(GUI * pGUI);//,IMBProtListener * listener);
    //IMBProtListener * lis;
    void closeProtocol();
    int getQ1();
    int enableAuto();
  private:
    struct termios termOptions;
    int fd;
    PollTimer * pPollTimer;
    PollReader * pPollIncoming;
    //void dispatcher(unsigned char ucDat);
  };
  
#endif /* __MBPROT_H__*/
}//end namespace
