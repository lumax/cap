/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __MBPROT_H__
#define __MBPROT_H__
  //#include "Poll.h"
  class PollReader;
  class PollTimer;

  class MBProtocol:IPollTimerListener,IPollReadListener
  {
  public:
    MBProtocol();
    
    virtual void pollReadEvent(PollSource * s);
    virtual void pollTimerExpired(long us);
    int initProtocol(GUI * pGUI);
    void closeProtocol();
    int getQ1();
  private:
    struct termios termOptions;
    int fd;
    PollTimer * pPollTimer;
    PollReader * pPollIncoming;
  };
  
#endif /* __MBPROT_H__*/
}//end namespace
