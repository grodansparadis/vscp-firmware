
/*
Copyright (C) 2008 Gediminas Simanskis , edevices, www.edevices.lt
*/

#ifndef __LISTS_H
#define __LISTS_H

#include "75x_lib.h"
#include "usb_lib.h"
#include "main.h"

/****************** Messages list ************************/

#define CAN_LIST_SIZE   64
#define CAN_LIST_MASK   CAN_LIST_SIZE - 1

typedef struct{  
canmsg CanMsgBuf[CAN_LIST_SIZE];
  u16 Head;
  u16 Tail;
  u16 Count;
}CanMsgList;

extern void CanListInit(CanMsgList *canmsglist);
extern void CanListClear(CanMsgList *canmsglist);
extern u16 CanListCount(CanMsgList *canmsglist);
extern bool CanListPut(CanMsgList *canmsglist,canmsg *CanMsg);
extern bool CanListGet(CanMsgList *canmsglist,canmsg *CanMsg); 

/*************** Commands lists *************************/

#define CAN_CMD_LIST_SIZE   2
#define CAN_CMD_LIST_MASK   CAN_CMD_LIST_SIZE - 1

typedef struct{
  u8  Channel;
  u8  Cmd;
  u8  NoOfData;
  u8  Data[28];
}cancmd;


////////////////////////////////////////////////////////////////
//   Kihtamieji
//

// Lists
extern CanMsgList  CanRXlist;
extern CanMsgList  CanTXlist;

extern CanMsgList  CanErrList;

extern canmsg  ErrorCanMsg;

#endif
