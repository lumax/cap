/*
Bastian Ruppert
*/

namespace EuMax01
{
#ifndef __ExaktG_H__
#define __ExaktG_H__

  class ExaktG:IPollReadListener,IPollTimerListener
  {
  public:
    ExaktG(int verbExakt,int verbGCtrl);
    ~ExaktG(){
      delete(pr_gcodes);
    }

    void setVerbose(int verb);
    virtual void (pollReadEvent)(PollSource * s);
    virtual void (pollTimerExpired)(long time);
    void setFD(int fd);
    PollReader* getPollReader(void);
    PollTimer* getPollTimer(void);
    G_Ctrl * getG_Ctrl(void);
    void setCoordinatesRelative(void);
    void setCoordinatesAbsolute(void);
    static void streamScanResult(struct StreamScanner_t * ps);

  private:
    PollReader * pr_gcodes;
    PollTimer * pt_gcodes;
    int verbose;
    G_Ctrl GCtrl;
    StreamScanner sScan;
  };

#endif /* __ExaktG_H__*/
}//end namespace
