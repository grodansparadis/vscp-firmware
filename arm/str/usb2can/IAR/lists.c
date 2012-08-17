

/*
Copyright (C) 2008 Gediminas Simanskis , edevices, www.edevices.lt
*/

#include "lists.h"

/******************** Messages lists *******************************/

void CanListInit(CanMsgList *canmsglist)
{
  canmsglist->Head = 0;
  canmsglist->Tail = 0;
}

void CanListClear(CanMsgList *canmsglist)
{
  canmsglist->Head = 0;
  canmsglist->Tail = 0;
}

bool CanListPut(CanMsgList *canmsglist,canmsg *CanMsg)
{
 u16  temp;
 bool res = FALSE;
   
      EIC_IRQCmd(DISABLE);
 
      temp = ( canmsglist->Head + 1 ) % CAN_LIST_SIZE;
      
      if( temp != canmsglist->Tail )
      {
        canmsglist->CanMsgBuf[temp] = *CanMsg;
        canmsglist->Head = temp; 
        canmsglist->Count++;
        res = TRUE;
      }
      else        
        res = FALSE;      
      
       EIC_IRQCmd(ENABLE);
       
   return  res;              
}

bool CanListGet(CanMsgList *canmsglist,canmsg *CanMsg)
{
  u16  temp; 
  bool res = FALSE;
  
     EIC_IRQCmd(DISABLE);
  
     if( canmsglist->Head != canmsglist->Tail )
     {        
       temp = ( canmsglist->Tail + 1 ) % CAN_LIST_SIZE;
       *CanMsg = canmsglist->CanMsgBuf[temp]; 
       canmsglist->Tail = temp;  
       canmsglist->Count--;
       res = TRUE;
     }
     else
       res = FALSE;     
     
     EIC_IRQCmd(ENABLE);
       
   return  res;             
}

u16 CanListCount(CanMsgList *canmsglist)
{
  return  canmsglist->Count; 
}

///////////////////////////////////////////////////////////////////////////////
//   Kintamieji
//

// CAN data lists
CanMsgList  CanRXlist;
CanMsgList  CanTXlist;

CanMsgList  CanErrList;

canmsg  ErrorCanMsg;
canmsg  RespCanMsg;
