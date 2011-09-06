/*
********************************************************************************
*                                                                              *
*   Filename:       protocol.c                                                 *
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

#include "protconfig.h"
#include "protocol.h"
#include <string.h>
#ifdef PRTPCMODE
#include "fw3main.h"
#else
#include "main.h"
#endif


volatile unsigned char ucPrtMsgLen;			// len of message frame
volatile unsigned char ucPrtCommand;		// last received command
volatile unsigned char ucPrtRxCrc;			// checksum
volatile unsigned char ucPrtMsgRxNo;		// last received message number
volatile unsigned char ucPrtMsgTxNo;		// last received message number
volatile unsigned char ucPrtState;       	// state of receiver statemachine
volatile unsigned char ucPrtByteCnt;     	// received bytes count
volatile unsigned char * ptrRcvBuf;      	// pointer to reiceve buffer
volatile unsigned char ucRcvBuf[nRCVBUFLEN];     	// receive buffer
volatile unsigned char ucPrtCRC;         	// CRC buffer
volatile unsigned char ucRcvtimout;      	// receiver timeout
volatile unsigned int uiCRCerr = 0;

volatile unsigned char ucAckreceived = 0;
volatile unsigned char ucAcktimout = 0;
volatile unsigned char ucLastMsgnr = 0;

volatile unsigned char ucPrtTimeouts;
volatile unsigned char ucPrtAckTimeouts; 

void prt_scan_framestart(unsigned char ucDat);
void prt_rcv_cmd(unsigned char ucDat);
void prt_rcv_len(unsigned char ucDat);
void prt_rcv_no(unsigned char ucDat);
void prt_rcv_dat(unsigned char ucDat);
void prt_rcv_crc(unsigned char ucDat);
void prt_sendack(void);
void prt_putchar(unsigned char ucDat);
void prt_init(void);
void prt_idle(void);


// calculate CRC serial, returns CRC 
unsigned char prt_crc(unsigned char ucCrc, unsigned char ucValue)
{
unsigned char ucI;
 	for (ucI=0; ucI<8; ucI++) 
 	{
  		ucCrc=(ucCrc>>1)^(((signed char)(ucValue^(ucCrc<<7))>>7)&0x8C);
  		ucValue<<=1;
 	}
	return(ucCrc);
}



// functionpointertable of receiver state machine

const(*prtTable[8])(unsigned char) = {	(const (*)(unsigned char))prt_init,		\
										(const (*)(unsigned char))prt_scan_framestart, 	\
										(const (*)(unsigned char))prt_rcv_cmd, 			\
								     	(const (*)(unsigned char))prt_rcv_len, 			\
										(const (*)(unsigned char))prt_rcv_no, 			\
										(const (*)(unsigned char))prt_rcv_dat, 			\
										(const (*)(unsigned char))prt_rcv_crc,			\
										(const (*)(unsigned char))prt_idle};


#ifdef PRTPCMODE
#define prtPutch        frmMain->m_CommThread->PutByte
#define prtDispatcher   frmMain->prt_dispatcher
#else
void(*prtDispatcher)(unsigned char);	// functionpointer to command dispatcher
void(*prtPutch)(unsigned char);			// functionpointer to putch
void(*extDispatcher)(unsigned char);

#endif

void prtmodule_init(void)				// init protocol engine
{
	PRTPROCESS(nPRTINIT);				// init statemachine
}

void prt_idle(void)
{
	PRTPROCESS(nPRTINIT);				// init statemachine
}

void prt_init(void)
{
	ptrRcvBuf = ucRcvBuf;				// set pointer to receive buffer
	ucPrtState = nPRTSCANHEADER;
	ucPrtMsgLen	= 5;
	ucPrtCommand = 0;
	ucPrtMsgRxNo = 0;
	ucPrtMsgTxNo = 0;
	ucPrtByteCnt = 0;
	ucPrtTimeouts = 0;
	ucPrtAckTimeouts = 0;
	ucPrtCRC = 0;
	ucRcvtimout = nRCVTIMOUT;
}

void prt_scan_framestart(unsigned char ucDat)
{
	if(ucDat == PRTHEADER)
	{
		ucPrtState++;
		ucPrtMsgLen	= 5;
		ucPrtByteCnt = 1;
		ucPrtCRC = prt_crc(ucPrtCRC,ucDat);
	}
	ucRcvtimout = nRCVTIMOUT;
}

void prt_jump_scan(void)
{
	ptrRcvBuf = ucRcvBuf;				// set pointer to receive buffer
	ucPrtState = nPRTSCANHEADER;
	ucPrtMsgLen	= 5;
	ucPrtCommand = 0;
	ucPrtByteCnt = 0;
	ucPrtCRC = 0;
	ucPrtAckTimeouts = 0;
	ucPrtTimeouts = 0;
	ucRcvtimout = nRCVTIMOUT;
}


// receive command
void prt_rcv_cmd(unsigned char ucDat)
{
	ucPrtCommand = ucDat & 0x7F;
	ucPrtState ++;
	ucPrtByteCnt ++;
	*ptrRcvBuf = ucPrtCommand;			// receive array index 0 contains command
	ptrRcvBuf++;
	ucPrtCRC = prt_crc(ucPrtCRC,ucDat);
	ucRcvtimout = nRCVTIMOUT;
}

// receive message len
void prt_rcv_len(unsigned char ucDat)
{
	ucPrtMsgLen = ucDat;

	if(ucPrtMsgLen > (nRCVBUFLEN - 1))
	{
		prt_jump_scan();
	}			
	else
	{
		ucPrtState ++;
		ucPrtByteCnt ++;
		ucPrtCRC = prt_crc(ucPrtCRC,ucDat);
		ucRcvtimout = nRCVTIMOUT;
	}
}

// receive message number
void prt_rcv_no(unsigned char ucDat)
{
	ucPrtMsgRxNo = ucDat;

	ucPrtByteCnt ++;
	ucPrtCRC = prt_crc(ucPrtCRC,ucDat);
	ptrRcvBuf = ucRcvBuf;				// set pointer to receive buffer

	if (ucPrtMsgLen - ucPrtByteCnt == 1)
	{
		ucPrtState += 2;                // skip data receive state
	}
	else
	{
	 ucPrtState ++;
	}

	ucRcvtimout = nRCVTIMOUT;
}

// receive containing data
void prt_rcv_dat(unsigned char ucDat)
{
	*ptrRcvBuf = ucDat;				// store received byte in buffer
	ptrRcvBuf++;						// next index

	if(ptrRcvBuf > (ucRcvBuf + nRCVBUFLEN - 1))
	{
		prt_jump_scan();
	}		
	else
	{
		ucPrtCRC = prt_crc(ucPrtCRC,ucDat);
		ucPrtByteCnt ++;					// inc receive byte count

		if((ucPrtMsgLen - ucPrtByteCnt) == 1)	// all bytes received ?
		{
			*ptrRcvBuf = 0;
			ucPrtState ++;					// next state
		}
		ucRcvtimout = nRCVTIMOUT;
	}		
}

// receive crc
void prt_rcv_crc(unsigned char ucDat)
{
	ucPrtRxCrc = ucDat;
	ucRcvtimout = nRCVTIMOUT;

	if(ucPrtCRC == ucPrtRxCrc)			// crc ok?
	{
		// validate message
		prtDispatcher(ucPrtCommand);	// call message dispatcher
	}
	else
	{
		uiCRCerr++;
	}
	ucLastMsgnr = ucPrtMsgRxNo;
	prt_jump_scan();	
}

void prt_timer(void)
{
	if( ucRcvtimout )                   // timeout reached ?
	{
		ucRcvtimout--;
	}
	else
	{
		prt_jump_scan(); 	        // init statemachine
		ucPrtTimeouts++;
	}

	if( ucAcktimout )
	{
		ucAcktimout--;
		ucPrtAckTimeouts++;
	}

}


// encapsulated putchar routine
void prt_putchar(unsigned char ucDat)
{
    prtPutch(ucDat);
}

unsigned char* prt_get_data(void)
{
	return (unsigned char *)ucRcvBuf;
}

unsigned char prt_get_length(void)
{
   return (unsigned char)ucPrtMsgLen;
}

void prt_sendack(void)
{
	unsigned char ucCrc;
	unsigned char ucIdx;
	unsigned char ucAckMsg[4]={PRTHEADER,PRTACK,0x05,0x0};
	unsigned char ucDat;

	ucAckMsg[3] = ucPrtMsgRxNo;	// store received message number

	ucCrc=0;								// clear crc

	for(ucIdx=0; ucIdx < 5; ucIdx++)	
	{
		if(ucIdx < 4)						// send data ?
		{
			ucDat = ucAckMsg[ucIdx];
			ucCrc = prt_crc(ucCrc,ucDat);
			prt_putchar(ucDat);
		}
		else									// send crc
		{
			prt_putchar(ucCrc);
		}
	}
}

void prt_sendmsg(unsigned char ucCmd, unsigned char* ptrMsg)
{

unsigned char ucLen;
unsigned char ucCrc=0;
	ucLen = strlen((char*)ptrMsg);
	ucLen += 5;								// add len for header, command, msg cnt, crc
	ucPrtMsgTxNo++;

	ucCrc = prt_crc(ucCrc,PRTHEADER);
	prt_putchar(PRTHEADER);

	ucCrc = prt_crc(ucCrc,ucCmd);
	prt_putchar(ucCmd);

	ucCrc = prt_crc(ucCrc,ucLen);
	prt_putchar(ucLen);

	ucCrc = prt_crc(ucCrc,ucPrtMsgTxNo);
	prt_putchar(ucPrtMsgTxNo);

	while (*ptrMsg)
	{
		ucCrc = prt_crc(ucCrc,*ptrMsg);
		prt_putchar(*ptrMsg);
		ptrMsg++;
	}

	prt_putchar(ucCrc);
}

void prt_sendmsg_uint(unsigned char ucCmd,unsigned int uiVal)
{

unsigned char ucLen;
unsigned char ucCrc=0;
unsigned char * ptrVal; 

	ptrVal = (unsigned char*)&uiVal;
	ucLen = 7;								// add len for header, command, msg cnt, crc
	ucPrtMsgTxNo++;

	ucCrc = prt_crc(ucCrc,PRTHEADER);
	prt_putchar(PRTHEADER);

	ucCrc = prt_crc(ucCrc,ucCmd);
	prt_putchar(ucCmd);

	ucCrc = prt_crc(ucCrc,ucLen);
	prt_putchar(ucLen);

	ucCrc = prt_crc(ucCrc,ucPrtMsgTxNo);
	prt_putchar(ucPrtMsgTxNo);

	ucCrc = prt_crc(ucCrc,*ptrVal);
	prt_putchar(*ptrVal);
	ptrVal++;

	ucCrc = prt_crc(ucCrc,*ptrVal);
	prt_putchar(*ptrVal);

	prt_putchar(ucCrc);

}
#ifdef ACKMODE
unsigned char  prt_sendmsg_uint_ack (unsigned char ucCmd,unsigned int uiVal)
{
unsigned char ucRpt = 3;

	while (ucRpt)
	{
		ucAckreceived = 0;
		prt_sendmsg_uint(ucCmd , uiVal);
		ucAcktimout = 20;
		
		while(ucAcktimout && (ucAckreceived == 0))
		{
			tsk_process_prot();
		};


		if(ucAckreceived)
		{
			return(1);
		}
		else if(ucRpt)
		{
			ucRpt--;
			ucPrtMsgTxNo--;			
		}		
		else
		{
			return(0);
		}
	}
	return(0);
}
#endif

void prt_sendmsg_int(unsigned char ucCmd,int iVal)
{

unsigned char ucLen;
unsigned char ucCrc=0;
unsigned char * ptrVal; 

	ptrVal = (unsigned char*)&iVal;
	ucLen = 7;								// add len for header, command, msg cnt, crc
	ucPrtMsgTxNo++;

	ucCrc = prt_crc(ucCrc,PRTHEADER);
	prt_putchar(PRTHEADER);

	ucCrc = prt_crc(ucCrc,ucCmd);
	prt_putchar(ucCmd);

	ucCrc = prt_crc(ucCrc,ucLen);
	prt_putchar(ucLen);

	ucCrc = prt_crc(ucCrc,ucPrtMsgTxNo);
	prt_putchar(ucPrtMsgTxNo);

	ucCrc = prt_crc(ucCrc,*ptrVal);
	prt_putchar(*ptrVal);
	ptrVal++;

	ucCrc = prt_crc(ucCrc,*ptrVal);
	prt_putchar(*ptrVal);

	prt_putchar(ucCrc);

}
#ifdef ACKMODE
unsigned char  prt_sendmsg_int_ack (unsigned char ucCmd,int iVal)
{
unsigned char ucRpt = 3;

	while (ucRpt)
	{
		ucAckreceived = 0;
		prt_sendmsg_int(ucCmd , iVal);
		ucAcktimout = 20;
		
		while(ucAcktimout && (ucAckreceived == 0))
		{
			tsk_process_prot();
		};
		
		if(ucAckreceived)
		{
			return(1);
		}
		else if(ucRpt)
		{
			ucRpt--;
			ucPrtMsgTxNo--;
		}		
		else
		{
			return(0);
		}
	}
	return(0);
}
#endif

void prt_sendmsg_long(unsigned char ucCmd,unsigned long ulVal)
{

unsigned char ucLen;
unsigned char ucCrc=0;
unsigned char * ptrVal; 

	ptrVal = (unsigned char*)&ulVal;
	ucLen = 9;								// add len for header, command, msg cnt, crc
	ucPrtMsgTxNo++;

	ucCrc = prt_crc(ucCrc,PRTHEADER);
	prt_putchar(PRTHEADER);

	ucCrc = prt_crc(ucCrc,ucCmd);
	prt_putchar(ucCmd);

	ucCrc = prt_crc(ucCrc,ucLen);
	prt_putchar(ucLen);

	ucCrc = prt_crc(ucCrc,ucPrtMsgTxNo);
	prt_putchar(ucPrtMsgTxNo);

	ucCrc = prt_crc(ucCrc,*ptrVal);
	prt_putchar(*ptrVal);
	ptrVal++;

	ucCrc = prt_crc(ucCrc,*ptrVal);
	prt_putchar(*ptrVal);
	ptrVal++;

	ucCrc = prt_crc(ucCrc,*ptrVal);
	prt_putchar(*ptrVal);
	ptrVal++;

	ucCrc = prt_crc(ucCrc,*ptrVal);
	prt_putchar(*ptrVal);

	prt_putchar(ucCrc);

}



void prt_sendblock(unsigned char ucCmd, unsigned char* ptrMsg, unsigned char ucLen)
{
unsigned char ucCrc=0;
unsigned char ucLength = ucLen;

	ucLength += 5;								// add len for header, command, msg cnt, crc
	ucPrtMsgTxNo++;

	ucCrc = prt_crc(ucCrc,PRTHEADER);
	prt_putchar(PRTHEADER);

	ucCrc = prt_crc(ucCrc,ucCmd);
	prt_putchar(ucCmd);

	ucCrc = prt_crc(ucCrc,ucLength);
	prt_putchar(ucLength);

	ucCrc = prt_crc(ucCrc,ucPrtMsgTxNo);
	prt_putchar(ucPrtMsgTxNo);

	while (ucLen)
	{
		ucLen--;
		ucCrc = prt_crc(ucCrc,*ptrMsg);
		prt_putchar(*ptrMsg);
		ptrMsg++;
	}

	prt_putchar(ucCrc);
}


