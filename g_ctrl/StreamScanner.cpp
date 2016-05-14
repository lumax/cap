/*
Bastian Ruppert
*/

//#include <sys/stat.h>
//#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "StreamScanner.h"

namespace EuMax01
{

  StreamScanner::StreamScanner():maxScans(16),			\
					 maxPayloadLen(16),	\
					 streamBufLen(256)
  {
    scanslen = 0;
  }

  void StreamScanner::scan(char c)
  {
    for(int i=0; i<this->scanslen;i++){
      scanOneByte(c,i);
    }
  }

  inline void StreamScanner::statePayload(char c, int index)
  {
     if(c == scans[index].pcDelim1[0] || c == scans[index].pcDelim2[0]){
       if(nStreamScannerType_int==scans[index].typeToScanFor||\
	  nStreamScannerType_float==scans[index].typeToScanFor){
	 scans[index].pcStreamBuf[scans[index].len] = '\0';
	 scans[index].scannedInt = atoi(&scans[index].pcStreamBuf[scans[index].preambleLen]);
	 scans[index].scannedFloat =					\
	   (float)atof(&scans[index].pcStreamBuf[scans[index].preambleLen]);
       }else{
	 scans[index].scannedInt = -1;
	 scans[index].scannedFloat = -1.0;
       }
       if(nStreamScannerType_G_fReturn == scans[index].typeToScanFor){
	 //printf("nStreamScannerType_G_fReturn: %s\n",&scans[index].pcStreamBuf[scans[index].preambleLen]);
	 char * pcTmp = 0;
	 pcTmp = &scans[index].pcStreamBuf[scans[index].preambleLen];
	 for(int i=0;i<4;i++){
	   if(pcTmp)
	     {
	       scans[index].scannedG_F[i] = atoi(pcTmp);
	     }
	   pcTmp = strpbrk(pcTmp,",");
	   pcTmp++;
	 }
       }
      if(0 != scans[index].fnkScanResult){
	(*scans[index].fnkScanResult)(&scans[index]);
      }
      resetScan(index);
    }else{
      scans[index].pcStreamBuf[scans[index].len] = c;
      scans[index].len++;
    }
    if(scans[index].len>=this->maxPayloadLen){
      resetScan(index);
    }
  }

  inline void StreamScanner::statePreamble(char c, int index)
  {
    if(c != scans[index].pcPreamble[scans[index].len]){
      resetScan(index);
    }
    else{
      scans[index].pcStreamBuf[scans[index].len] = c;
      scans[index].len++;
    }
    if(scans[index].len >= (int)scans[index].preambleLen){
      scans[index].state = nStreamScannerState_payload;
    }
  }

  inline void StreamScanner::scanOneByte(char c,int index)
  {
    switch(scans[index].state){
    case nStreamScannerState_payload:{
      statePayload(c,index);
      break;
    }
    default:{//nStreamScannerState_preamble
      statePreamble(c,index);
      break;
    }
    }
  }

  int StreamScanner::addScanner(int type,				\
				void * userPnt,				\
				int userID,				\
				const char * preamble,			\
				char * delim1,				\
				char * delim2,				\
				void (*fnkScanRes)(struct StreamScanner_t *))
  {
    /*noch Platz?*/
    if(this->scanslen>=this->maxScans){
      return -1;
    }
    /*Passen die Parameter?*/
    if(0==preamble){
      return -2;
    }
    if(0==delim1 && 0==delim2){
      return -3;
    }
    if(this->streamBufLen <= (int)(strlen(preamble) + this->maxPayloadLen + 1/*delim*/)){
      return -4;
    }

    /*Type: im Zweifel immer Float*/
    if(type >= nStreamScannerType_LAST_NUMBER || type < 0)
      {
	scans[this->scanslen].typeToScanFor = nStreamScannerType_float;
      }
    else
      {
	scans[this->scanslen].typeToScanFor = type;
      }
    scans[this->scanslen].userPnt = userPnt;
    scans[this->scanslen].userID = userID;
    scans[this->scanslen].pcPreamble = preamble;
    scans[this->scanslen].pcDelim1 = delim1;
    scans[this->scanslen].pcDelim2 = delim2;
    if(0==delim1){
      scans[this->scanslen].pcDelim1 = delim2; 
    }
    if(0==delim2){
      scans[this->scanslen].pcDelim2 = delim1; 
    }
    scans[this->scanslen].fnkScanResult = fnkScanRes;

    scans[this->scanslen].preambleLen = strlen(scans[this->scanslen].pcPreamble);

    resetScan(this->scanslen);

    for(int i = 0;i < this->streamBufLen;i++){
	scans[this->scanslen].pcStreamBuf[i] = 0x00;
    }

    scanslen++;

    return 0;
  }

  inline void StreamScanner::resetScan(int index)
  {
    scans[index].state = nStreamScannerState_preamble;
    scans[index].len = 0;   
  }
}
