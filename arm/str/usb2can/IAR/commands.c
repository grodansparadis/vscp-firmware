
#include "commands.h"


void  USB2CAN_reset(void)
{
  Reset_Device();  
}

void  USB2CAN_set_speed(u8 speed,CAN_TIMINGS *CanTimings)
{
  CAN_InitTypeDef     CAN_Struct;
  
  switch(speed)
  {
  case    CAN_BAUD_10:    
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_10K;  
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );  
    break;
  case    CAN_BAUD_20:
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_20K;
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );        
    break;
  case    CAN_BAUD_50:
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_50K;
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );        
    break;
  case    CAN_BAUD_100:    
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_100K;
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );        
    break;
  case    CAN_BAUD_125:  
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_125K;
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );        
    break;
  case    CAN_BAUD_250:   
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_250K;
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );        
    break;
  case    CAN_BAUD_500:   
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_500K;
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );        
    break;
  case    CAN_BAUD_800:   
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_800K;    
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );        
    break;
  case    CAN_BAUD_1000:   
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_1M;    
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );        
    break;
  case    CAN_BAUD_MANUAL:        
      //printf("tseg1=%d\r\n",CanTimings->tseg1);
      //printf("tseg2=%d\r\n",CanTimings->tseg2);
      //printf("sjw1=%d\r\n",CanTimings->sjw);
      //printf("brp=%d\r\n",CanTimings->brp);
     
      CAN_EnterInitMode(CAN_CR_CCE | CAN_CR_IE);
      CAN_SetTiming(CanTimings->tseg1,CanTimings->tseg2,CanTimings->sjw,CanTimings->brp);
      CAN_LeaveInitMode();    
    break;
  default: // 125K   
      CAN_Struct.CAN_Bitrate=CAN_BITRATE_125K;
      CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;
      CAN_Init( &CAN_Struct );        
    break;
  }  
}

void  USB2CAN_open(u32 speed,u32 flags,CAN_TIMINGS *CanTimings)
{
   CAN_ENABLE();    
   USB2CAN_set_speed(speed,CanTimings);

  // pasiziurime mode registra
   switch( flags & 0x0003 )
   {
   case  CAN_MODE_NORMAL: // normal mode         
      CAN_LeaveTestMode(); 
    break;
   case  CAN_MODE_SILENT: // silent mode
      CAN_EnterTestMode( CAN_TESTR_SILENT );    
    break;
   case  CAN_MODE_LOOPBACK: // loopback mode
      CAN_EnterTestMode( CAN_TESTR_LBACK );    
     break;
   case  CAN_MODE_SILENT_LOOPBACK:
      CAN_EnterTestMode( CAN_TESTR_LBACK | CAN_TESTR_SILENT );
     break;
   default:
      CAN_LeaveTestMode(); 
     break;
   }        
    // DAR
     //if( flags & 0x0004 )
      //CAN->CR |= CAN_CR_DAR; // disable auto retransmitions           
     //else     
      //CAN->CR &= ~CAN_CR_DAR; // enable auto retransmitions  
}

////////////////////////////////

void  USB2CAN_close(void)
{
  CAN_DISABLE();
}

////////////////////////////////

u32  USB2CAN_get_status(void)
{
  u32  res = 0;
  u32  status = CAN->SR; 
  u32  rxtxcnt = CAN->ERR;
  
  if( status & CAN_SR_BOFF )
  {
   res |= CANAL_STATUS_BUS_OFF ;
  }
  
  if( status & CAN_SR_EWARN )
  {
   res |= CANAL_STATUS_BUS_WARN ;       
  }
  
  if( status & CAN_SR_EPASS )
  {
   res |= CANAL_STATUS_PASSIVE ;
  }
    
  res |= (((rxtxcnt >> 8) & 7) + (( rxtxcnt >> 15 ) & 1) << 8 ); // RX cnt 
  res |= ( rxtxcnt & 8 );      // TX cnt
     
  return res;
}

////////////////////////////////
/*
typedef struct{
  u32   ReceivedFrames;
  u32   TransmitedFrames;
  u32   ReceivedDataBytes;
  u32   TransmitedDataBytes;
  u32   Overruns;
  u32   Warnings;
  u32   Errors;
}
STATISTICS;
*/

u32  USB2CAN_get_statistics( u8 num )
{
  u32  res = 0;
  
  switch( num )
  {
  case   0:  
    res = Statistics.ReceivedFrames;
    break;
  case   1:
    res = Statistics.TransmitedFrames;
    break;
  case   2:
    res = Statistics.ReceivedDataBytes;
    break;
  case   3:
    res = Statistics.TransmitedDataBytes;
    break;
  case   4:
    res = Statistics.Overruns;
    break;
  case   5:
    res = Statistics.Warnings;
    break;
  case  6:
    res = Statistics.BusOff;
    break;          
  default:
    break;
  }
  
  StatisticsInit();
  
  return res;
}
