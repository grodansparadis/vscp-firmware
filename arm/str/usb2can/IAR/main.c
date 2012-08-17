
#include "75x_lib.h"
#include "stdio.h"
#include "main.h"
#include "lists.h"
#include "string.h"
#include "leds.h"
#include "i2c_ee.h"

//USB includes
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "messages.h"
#include "configuration.h"
#include "buffers.h"

u8 buffer_cmd_out[VIRTUAL_COM_PORT_DATA_SIZE];
u8 buffer_cmd_in[VIRTUAL_COM_PORT_DATA_SIZE];
volatile u8 count_cmd_out = 0;
volatile u8 count_cmd_in = 0;

u8 buffer_data_out[VIRTUAL_COM_PORT_DATA_SIZE];
u8 buffer_data_in[VIRTUAL_COM_PORT_DATA_SIZE];
volatile u8 count_data_out = 0;
volatile u8 count_data_in = 0;

extern volatile u32 GlobalTick;

// ERROR & status variables
u32   ErrTimerTick;
u8    ErrStatus = 0;
u8    ErrStatusNext = 0;
u8    ErrLec = 0;
u8    ErrLecNext = 0;

STATISTICS Statistics;

// USB2CAN state . open,closed
volatile bool bRun = FALSE;
volatile bool bRunNext = FALSE;

// error states
//volatile bool bBusOff = FALSE;
//volatile bool bBusWarn = FALSE;
//volatile bool bErrorPassive = FALSE;

// last read CAN traceiver status
volatile u32 USB2CAN_status;
volatile u32 USB2CAN_next_status;

canmsg      TxCanMsg;
canmsg      RxCanMsg;
canmsg      TmpCanMsg;
bool        TxCanFlag = FALSE;
bool        RxCanFlag = FALSE;

///////////////////////////////////////////////////////////////////////////////
// Statistics Init
//

void StatisticsInit(void)
{
  Statistics.ReceivedFrames = 0;
  Statistics.TransmitedFrames = 0;
  Statistics.ReceivedDataBytes = 0;
  Statistics.TransmitedDataBytes = 0;
  Statistics.Overruns = 0;
  Statistics.BusOff = 0;
}

u16 usb_tx_index = 0;  
vu16 total_len = 0;

bool EP1_busy = FALSE;
bool EP2_dataReady = FALSE;

///////////////////////////////////////////////////////////////////////////////
// main
//

int main()
{  
  // local buffer for USB TX

//  u16 x;
//  u8  buffer[65];
  //u16 index = 0;  
  
  bool tmp_bRunNext,tmp_bRun;
  
  GlobalTick = 0; 
  
  #ifdef DEBUG
    debug();
  #endif        
        
  Set_System();
  Set_TB();  
  Set_USBClock();  
  USB_Interrupts_Config();
  
  setBlinkType( LED_TOGGLE_FAST );    
  USB_Init();    
  CAN_Config();
  
  CanListInit(&CanRXlist);
  CanListInit(&CanTXlist);
  CanListInit(&CanErrList);
  
  // error and response messages
  ErrorCanMsg.IdType = 0;

  // Error msg init  
  ErrorCanMsg.Id = 0;
  ErrorCanMsg.Dlc = 0;
  ErrorCanMsg.Timestamp = 0;  
  ErrTimerTick = 0;
  
  setBlinkType( LED_RED_STEADY );
   
  while(1)
  {    
    
//////////////// USB2CAN state changes //////////////////////    
    
    tmp_bRunNext = bRunNext;
    tmp_bRun = bRun;
    
    if( tmp_bRunNext != tmp_bRun )
    { 
      if( bRunNext == TRUE )            
        setBlinkType( LED_GREEN_STEADY  );                   
      else            
        setBlinkType( LED_RED_STEADY );                 
      
     // betkoks busenu pasikeitimas
     CanListClear( &CanTXlist );
     CanListClear( &CanRXlist ); 
     CanListClear( &CanErrList );
     ErrStatus = 0;
     ErrStatusNext = 0;
     ErrLec = 0;
     ErrLecNext = 0;
     USB2CAN_status = 0;
     Statistics.ReceivedFrames = 0;
     Statistics.TransmitedFrames = 0;
     Statistics.Overruns = 0; 
     
     bRun = bRunNext;                    
    } 
    
//////////////////////////////////////////////////////////////////////////
//  if RUN
//    
    if( bRun == FALSE )   
      continue;

//////////////////////////////////////////////////////////////////////// 
//   USB2CAN EROR state changes  
//  
    
     USB2CAN_status = CAN->SR; 
     
     ErrStatusNext = (u8) (( USB2CAN_status >> 5 )& 0x07);    
     ErrLecNext = (u8) ( USB2CAN_status & 7 );   // LEC bits only

     if ( ErrStatus ^ ErrStatusNext )
     {                        
        if( ErrStatusNext & USB2CAN_BOFF )
        {
          Statistics.BusOff++;
          setBlinkType( LED_RED_BLINK_FAST ); 
          SendErrorMessage( CANAL_STATUSMSG_BUSOFF );
        }
        else if( ErrStatusNext & USB2CAN_EPASS )
        {          
          setBlinkType( LED_GREEN_BLINK_FAST );           
          SendErrorMessage( CANAL_STATUSMSG_BUSHEAVY );
        }
        else if( ErrStatusNext & USB2CAN_EWARN )        
        {
          Statistics.Warnings++;
          setBlinkType( LED_GREEN_BLINK_SLOW );
          SendErrorMessage( CANAL_STATUSMSG_BUSLIGHT );
        }
        else        
        {
          setBlinkType( LED_GREEN_STEADY );
          SendErrorMessage( CANAL_STATUSMSG_OK ); 
        }
      
      ErrStatus = ErrStatusNext;
     }       
     
     if( ErrLec ^ ErrLecNext )
     {       
       switch ( ErrLecNext )
       {         
       case   LEC_NO_ERROR:
         //SendErrorMessage( CANAL_STATUSMSG_OK ); 
         break;
       case   LEC_STUFF_ERROR:
         SendErrorMessage ( CANAL_STATUSMSG_STUFF );
         break;
       case   LEC_FORM_ERROR:
         SendErrorMessage( CANAL_STATUSMSG_FORM );
         break;
       case   LEC_ACK_ERROR:
         SendErrorMessage( CANAL_STATUSMSG_ACK );
         break; 
       case   LEC_BIT1_ERROR:
         SendErrorMessage( CANAL_STATUSMSG_BIT1 );
         break;
       case   LEC_BIT0_ERROR:
         SendErrorMessage( CANAL_STATUSMSG_BIT0 );
         break;
       case   LEC_CRC_ERROR:
         SendErrorMessage( CANAL_STATUSMSG_CRC );
         break; 
       default:
         break;         
       }
             
      ErrLec = ErrLecNext; 
     }
     
///////////////////////////////////////////////////////////////////////////
//  from USB to CAN 
//   
          
      if( TxCanFlag == FALSE )
         {
           if( CanListGet(&CanTXlist,&TxCanMsg )) 
           {      
             TxCanFlag = TRUE;                     
           }  
         }
         else
         {     
           if( TxCanMsg.IdType & CAN_EXT_ID )
            {                      
             if(!CAN_IsTransmitRequested (30)) // ziurim ar laisva
             {                   
              while( CAN_SendMessage( 30,&TxCanMsg) != SUCCESS );
              TxCanFlag = FALSE;        
             }
             else if(!CAN_IsTransmitRequested (31)) // ziurim ar laisva
             {
              while( CAN_SendMessage( 31,&TxCanMsg) != SUCCESS );
              TxCanFlag = FALSE;         
             }
           }
           else
           {
             if(!CAN_IsTransmitRequested (28)) // ziurim ar laisva 
             {                  
              while( CAN_SendMessage( 28,&TxCanMsg) != SUCCESS );
              TxCanFlag = FALSE;
             }
             else if(!CAN_IsTransmitRequested (29)) // ziurim ar laisva             
             {             
              while( CAN_SendMessage( 29,&TxCanMsg) != SUCCESS );
              TxCanFlag = FALSE;         
             }
           }   
         } // TxCanFlag...
             
////////////////////////////////////////////////////////////////////////
//   from CAN to USB data frames
//
   
     if( EP1_busy == FALSE ) // ziurim ar tuscias USB buferis
     {
       if( CanListGet(&CanRXlist,&RxCanMsg ))       
       {
         //Statistics.TransmitedFrames++;         
         CANtoUSBdata( &RxCanMsg );
         EP1_busy = TRUE;
       }      
     }   

//////////////////////////////////////////////////////////////////////
//   from USB to CAN buffer
//     
     
     if( EP2_dataReady == TRUE )
     {
        if( USBdataCANmsg(&TmpCanMsg) )
        {    
           if( CanListPut(&CanTXlist,&TmpCanMsg) )      
           {
             Statistics.TransmitedFrames++;
             Statistics.TransmitedDataBytes |= TmpCanMsg.Dlc;
             EP2_dataReady = FALSE; 
             SetEPRxValid(ENDP2); 
           }          
           else              
             Statistics.Overruns++;                 
       }
     }
     
//////////////////////////////////////////////////////////////////////
//   Error messages to USB 
//            
     // siuncia ERROR message kas 1 sek.
/*     if( GlobalTick > ( ErrTimerTick + 5000))
     {
      ErrorCanMsg.IdType = (u16)( DATA_FRAME_ERROR << 8 );
      CANtoUSBdata( &ErrorCanMsg );
      ErrorCanMsg.IdType = 0;
      ErrorCanMsg.Id = USB2CAN_status;
      ErrorCanMsg.Data[0] = (u8) CAN->ERR;
      ErrorCanMsg.Data[1] = (u8) (CAN->ERR >> 8) & 0x7F;
      ErrorCanMsg.Dlc = 2;
      GlobalTick = ErrTimerTick;      
     }*/     
          
  }// while... 

}// main...


  