
#ifndef __BUFFERS_H
#define __BUFFERS_H

#include "75x_lib.h"
#include "usb_lib.h"

#define BUFF_SIZE   256
#define BUFF_MASK   BUFF_SIZE - 1

typedef struct{  
  u8    Buff[BUFF_SIZE];
  vu16   Head;
  vu16   Tail;
  vu16   Count;
}USB_buffers_List;


void USB_buffers_Init(USB_buffers_List *USB_buff_list);
void USB_buffers_Clear(USB_buffers_List *USB_buff_list);
bool USB_buffers_Put(USB_buffers_List *USB_buff_list,u8 ch);
bool USB_buffers_Get(USB_buffers_List *USB_buff_list,u8 *ch);
u16 USB_buffers_Count(USB_buffers_List *USB_buff_list);

extern USB_buffers_List  USB_TX_buf;

#endif

