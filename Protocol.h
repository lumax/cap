/*
********************************************************************************
*                                                                              *
*   Filename:       protocol.h                                                 *
*   Description:	Serial Transfer Protocol								   *
*                                                                              *
********************************************************************************
*                                                                              *
*   Author:         Bruch Michael                                              *
*   Company:        MBE Michael Bruch                                          *
*                                                                              *
*   Revision:       1.30                                                       *
*   Date:           12.06.2007                                                 *
*                                                                              *
********************************************************************************
*/

#ifdef __cplusplus
extern "C" {
#endif

// state definition of protocol engine
enum PRTSTATES {nPRTINIT, nPRTSCANHEADER, nPRTRCVCMD, nPRTRCVLEN, nPRTRCVNO, nPRTRCVDAT, nPRTRCVCRC};


#define PRTHEADER		0x1F
#define PRTACK			0x7F

#define PRTPROCESS 		prtTable[ucPrtState & 0x07]
#define PRTDISPATCHER 	prtDispatcher

/*
********************************************************************************
*                                                                              *
*   Filename:       protocol.h                                                 *
*   Description:	Serial Transfer Protocol								   *
*                                                                              *
********************************************************************************
*                                                                              *
*   Author:         Bruch Michael                                              *
*   Company:        MBE Michael Bruch                                          *
*                                                                              *
*   Revision:       1.20                                                       *
*   Date:           20.06.2006                                                 *
*                                                                              *
********************************************************************************
*/

#define PRTPUTCH		prtPutch

#define nRCVTIMOUT  200      // 100 ms timeout value
#define nRCVBUFLEN  32
#define nRCVBUFCNT 	1
#define ACKMODE 1

extern volatile unsigned char ucAckreceived;
extern volatile unsigned char ucPrtState;
extern const void (*prtTable[8])(unsigned char);	// functionpointertable
extern void(*prtDispatcher)(unsigned char);		// functionpointer to command dispatcher
extern void(*prtPutch)(unsigned char);			// functionpointer to putch
extern void(*extDispatcher)(unsigned char);

void prtmodule_init(void);

void prt_timer(void);

void prt_sendmsg(unsigned char ucCmd, unsigned char* ptrMsg);
unsigned char* prt_get_data(void);
unsigned char prt_get_length(void);
void prt_sendmsg_int(unsigned char ucCmd,int iVal);
void prt_sendmsg_uint(unsigned char ucCmd,unsigned int uiVal);
void prt_sendmsg_long(unsigned char ucCmd,unsigned long ulVal);
void prt_sendblock(unsigned char ucCmd, unsigned char* ptrMsg, unsigned char ucLen);
void prt_sendack(void);
#ifdef ACKMODE
unsigned char  prt_sendmsg_int_ack (unsigned char ucCmd,int iVal);
unsigned char  prt_sendmsg_uint_ack (unsigned char ucCmd,unsigned int uiVal);
#endif

#ifdef __cplusplus
}
#endif
