/*
Bastian Ruppert
*/

namespace EuMax01
{
#ifndef __StreamScanner_H__
#define __StreamScanner_H__

  enum StreamScannerTypes{
    nStreamScannerType_int = 0,
    nStreamScannerType_float = 1,
    nStreamScannerType_G_fReturn = 2,
    nStreamScannerType_LAST_NUMBER
  };

  enum StreamScannerStates{
    nStreamScannerState_preamble = 0,
    nStreamScannerState_payload = 1,
  };

  struct StreamScanner_t{
    int typeToScanFor;
    int state;
    void * userPnt;
    int userID;
    const char * pcPreamble;//Startstring
    size_t preambleLen;//Länge Startstring
    char * pcDelim1;//nur ein Zeichen
    char * pcDelim2;//nur ein Zeichen
    char pcStreamBuf[256];
    int len;
    int scannedInt;
    float scannedFloat;
    int scannedG_F[4];
    void (*fnkScanResult)(struct StreamScanner_t *);
  };

  class StreamScanner
  {
 public:
  StreamScanner();
  ~StreamScanner(){}

  void scan(char c);
  int addScanner(int typeToScanFor,			\
		 void * userPnt,			\
		 int userID,				\
    const char * preamble,				\
    char * delim1,					\
    char * delim2,\
    void (*fnkScanRes)(struct StreamScanner_t * pss));

  private:
    struct StreamScanner_t scans[16];
    int scanslen;
    const int maxScans;//16
    const int maxPayloadLen;//16
    const int streamBufLen;//256, Buffer in struct StreamScanner_t
    inline void scanOneByte(char c,int index);
    inline void resetScan(int index);
    inline void statePreamble(char c, int index);
    inline void statePayload(char c, int index);
  };

#endif /* __StreamScanner_H__*/
}//end namespace
