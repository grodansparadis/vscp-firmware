
#include "lists.h"
#include "messages.h"
#include "string.h" 
#include "main.h"
#include "hw_config.h"
#include "commands.h"


////////////////////////////////////////////////////////////////////////////////
//   DATA messages
//

bool  USBdataCANmsg(canmsg *CanMsg)
{
  u8      DataLen;
  
  count_data_out = GetEPRxCount(ENDP2);
  PMAToUserBufferCopy(buffer_data_out, ENDP2_BUFF1_ADDR, count_data_out);
  
  if( bRun == FALSE )    
    return FALSE;       
  
  if( (buffer_data_out[0] != 0x55) || (buffer_data_out[19] != 0xAA) || (count_data_out > 60) )     
    return FALSE;
   
  
  DataLen = buffer_data_out[16];
  
  if(DataLen > 8)
      return  FALSE;       

  CanMsg->IdType = 0; // data message  
  
  if( buffer_data_out[2] & CAN_EXT_ID ) // EXT
  {
   CanMsg->IdType |= buffer_data_out[2] & 0x03; // RTR & EXT_ID
      
   CanMsg->Id = 
   (((u32)buffer_data_out[4]<<24) & 0x1f000000) |
   (((u32)buffer_data_out[5]<<16) & 0x00ff0000) |
   (((u32)buffer_data_out[6]<<8 ) & 0x0000ff00) |
   (((u32)buffer_data_out[7]    ) & 0x000000ff) ;
  }
  else // STD
  {
   CanMsg->IdType = buffer_data_out[2] & 0x03; // RTR & STD_ID
   CanMsg->Id = 0; 
   CanMsg->Id = 
   (((u32)buffer_data_out[6]<<8 ) & 0x00007f00) |
   (((u32)buffer_data_out[7]    ) & 0x000000ff) ;  
  }
  
    memcpy(&CanMsg->Data,&buffer_data_out[8],DataLen);
    CanMsg->Dlc = buffer_data_out[16];     
    
   //SetEPRxValid(ENDP2); 
    
    return  TRUE;
}    

////////////////////////////////////////////////////

u8  GetUSBdataCount(void)
{
  return count_data_in;
}

///////////////////////////////////////////////////

void ClearUSBdataBuffer(void)
{
  count_data_in = 0; 
}

///////////////////////////////////////////////////

void  CANtoUSBdata( canmsg *msg )
{
  u8  sendData[65];
  u8  size = 0;
         
	sendData[ size++ ] = 0x55; // FrameBegin
        sendData[ size++ ] = 0;    // Channel        
        sendData[ size++ ] = (u8)( msg->IdType & 0xFF );  // RTR and EXT_ID flags only 
        sendData[ size++ ] = (u8)( msg->IdType >> 8 );  // Data frame type (aprasyta messages.h)        
	sendData[ size++ ] = (u8)( msg->Id >> 24 ) & 0x1f;
	sendData[ size++ ] = (u8)( msg->Id >> 16 ) & 0xff;
	sendData[ size++ ] = (u8)( msg->Id >> 8 )  & 0xff;
	sendData[ size++ ] = (u8)( msg->Id ) & 0xff; // Message ID
			
	memcpy( sendData + 8, msg->Data,msg->Dlc  ); // DATA
	size += 8;                        
        sendData[ size++ ] = msg->Dlc; // NoOfData
        		
	sendData[ size++ ] = (u8)( msg->Timestamp >> 24 ) & 0x1f;
	sendData[ size++ ] = (u8)( msg->Timestamp >> 16 ) & 0xff;
	sendData[ size++ ] = (u8)( msg->Timestamp >> 8 )  & 0xff;
	sendData[ size++ ] = (u8)( msg->Timestamp ) & 0xff;  // Message TIMESTAMP
        
        sendData[ size++ ] = 0;    // SequenceNr
        sendData[ size++ ] = 0;    // CRC
        sendData[ size++ ] = 0xAA; // FrameEnd                      
                       
        UserToPMABufferCopy(sendData,ENDP1_BUFF1_ADDR,size);
        SetEPTxCount(ENDP1,size);
        SetEPTxValid(ENDP1);                           
}

///////////////////////////////////////////////////////////////////////////////
//    CMD messages
//

////////////////////////////////////////////////////

u8  GetUSBcmdCount(void)
{
  return count_cmd_in;
}

///////////////////////////////////////////////////

void ClearUSBcmdBuffer(void)
{
  count_cmd_in = 0; 
}

///////////////////////////////////////////////////

void GetUSBcmd(void)
{
  u8  channel;
  u8  command;
  u8  opt1;
  u8  opt2;
  u32 flags;
  u8  size;
  CAN_TIMINGS   cantimings;  
  u16 tmp;
  u8  buff[20];
  u32 res;
  
  //u8  tmp_blink_type = blink_type;
  //blink_type = LED_OFF;
  
  count_cmd_out = GetEPRxCount(ENDP4);
  PMAToUserBufferCopy( buffer_cmd_out,0x1D0 /*ENDP4_RXADDR*/, count_cmd_out);
    
  size = count_cmd_out;
  memcpy(buff,buffer_cmd_out,16);
  
  if( (buff[0] != 0x11) || (buff[15] != 0x22) || (size > 16) )
  {
   SetEPRxValid(ENDP4);
   return;
  }  
    channel = buff[1];
    command = buff[2];
    opt1    = buff[3];   
    opt2    = buff[4];
    
  switch( command )
  {
  case    USB2CAN_RESET:            
     opt1 = USB2CAN_CMD_SUCCESS;          
    break;
  case    USB2CAN_OPEN:
    
    cantimings.tseg1 = buff[5];
    cantimings.tseg2 = buff[6];
    cantimings.sjw =   buff[7];     
    cantimings.brp =   buff[8];        
    cantimings.brp = (cantimings.brp << 8) | buff[9];    
    
    flags = buff[10];
    flags = (flags << 8)| buff[11];
    flags = (flags << 8)| buff[12];
    flags = (flags << 8)| buff[13];
    
    USB2CAN_open(opt1,flags,&cantimings);// opt1 - speed
    bRunNext = TRUE; // indikacija           
    opt1 = USB2CAN_CMD_SUCCESS;         
    
    break;    
  case    USB2CAN_CLOSE:

     USB2CAN_close();     
     bRunNext = FALSE;
     opt1 = USB2CAN_CMD_SUCCESS;     
    
    break;    
  case    USB2CAN_SET_SPEED:
    
    if( bRun )
    {  
     USB2CAN_set_speed(opt1,&cantimings);
     opt1 = USB2CAN_CMD_SUCCESS;
    }
    else
      opt1 = USB2CAN_CMD_ERROR; 
    
    break;
  case    USB2CAN_SET_MASK_FILETR:
    break;
  case    USB2CAN_GET_STATUS:
    
    if( res = USB2CAN_get_status())
    {
      buff[5] = (u8) ( res >> 24 );
      buff[6] = (u8) ( res >> 16 );
      buff[7] = (u8) ( res >> 8 );
      buff[8] = (u8)  res ; 
      opt1 = USB2CAN_CMD_SUCCESS;
    }
    else
      opt1 = USB2CAN_CMD_ERROR;
    
    break;
  case    USB2CAN_GET_STATISTICS:
        
    if( res = USB2CAN_get_statistics(opt1))
    {
     buff[5] = (u8) ( res >> 24 );
     buff[6] = (u8) ( res >> 16 );
     buff[7] = (u8) ( res >> 8 );
     buff[8] = (u8)  res ;
     opt1 = USB2CAN_CMD_SUCCESS;
    }
    else
      opt1 = USB2CAN_CMD_ERROR;
    
    break;
  case    USB2CAN_GET_SERIAL:
    
    memcpy( &buff[5],&conf.Virtual_Com_Port_StringSerial[0],8 );
    opt1 = USB2CAN_CMD_SUCCESS;
    
    break;
  case    USB2CAN_GET_SOFTW_VER:
    
    buff[5] = conf.fver_maj;
    buff[6] = conf.fver_min;
      
    opt1 = USB2CAN_CMD_SUCCESS;
    
    break;
  case    USB2CAN_GET_HARDW_VER:
    
    buff[5] = conf.hver_maj;
    buff[6] = conf.hver_min;
    opt1 = USB2CAN_CMD_SUCCESS;
    
    break;
  case    USB2CAN_RESET_TIMESTAMP:
    break;
  default:
     opt1 = USB2CAN_CMD_ERROR; 
    break;    
  }
  
  buff[0] = 0x11;
  buff[1] = channel;
  buff[2] = command;
  buff[3] = opt1;
  buff[4] = opt2;
  buff[15] = 0x22;
 
  memcpy(buffer_cmd_in,buff,16);
  
//  while( count_cmd_in != 0 );  
  count_cmd_in = size;
  
  /****** issiunciam *****/
  UserToPMABufferCopy(buffer_cmd_in,0x1C0/*ENDP3_TXADDR*/, count_cmd_in);
  SetEPTxCount(ENDP3,count_cmd_in);
  SetEPTxValid(ENDP3);  

  SetEPRxValid(ENDP4); //leidziam vel priimti  
  
  if( command == USB2CAN_RESET )
  {
   tmp = 65000; 
   while(--tmp);
   USB2CAN_reset();   
  }
}

bool SendErrorMessage( u8 status )
{
  canmsg msg; 
  u32  status_cnt = CAN->ERR;
  
  msg.IdType = 0x0304; // error
  msg.Id = 0; // must be '0'
  msg.Dlc = 4;
  msg.Data[0] = status;
  msg.Data[1] = (u8) (  ((status_cnt >> 8) & 0x7F) + (( status_cnt >> 15) & 0x01 )  );   // REC
  msg.Data[2] = (u8) status_cnt; // TEC
  msg.Data[3] = 0;

   if( CanListPut(&CanRXlist,&msg))
     return TRUE;
   else  
     return FALSE;
}
