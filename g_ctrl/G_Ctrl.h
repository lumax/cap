/*
Bastian Ruppert
*/

namespace EuMax01
{
#ifndef __G_CTRL_H__
#define __G_CTRL_H__

  enum AXIS{
    nTinyG_X = 0,
    nTinyG_Y = 1,
    nTinyG_Z = 2,
    nTinyG_A = 3
  };

  class G_Ctrl
  {
  public:
    G_Ctrl(int verb);
    ~G_Ctrl();

    void setVerbose(int verb);
    void cmdFlowControl(void);
    void cmdG(char * gCode);
    void cmdG1(int axis,int range,int velocity);
    void cmdG1(int axis,float range,int velocity);
    void cmdG1(int axis,float range, int velocity,	\
	       int axis2,float range2);
    int openUart(char * path,int baud);
    void closeUart(void);
    int getFd(void);
    void cmdGetStatus(void);
  private:
    void sendCmd(char * cmd);
    int verbose;
    int cmdLen;
    char cmdBuf[128];
    char* getAxis(int axis);
    int fd;
  };
  
#endif /* __G_CTRL_H__*/
}//end namespace
