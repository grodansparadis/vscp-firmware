#include "75x_lib.h"
#include "rxtask.h"
#include "main.h"
#include "stdio.h"

u8  RxChar;
u8  state_incoming;
u8  substate_incoming; 

RXTX_UART_FRAME     RxUartFrame;
RXTX_UART_FRAME     TxUartFrame;

RXTX_UART_FRAME     RxUartFrameList[UART_RX_BUFF_SIZE];
RXTX_UART_FRAME     TxUartFrameList[UART_RX_BUFF_SIZE];
u8                  RxUartFrameListHead;
u8                  RxUartFrameListTail;
u8                  TxUartFrameListHead;
u8                  TxUartFrameListTail;
RXTX_UART_FRAME     TempFrame;

///////////////////////////////////////////////////////////////////////////////
//  Rx Tx tasks init
//

void RxTxTasksInit(void)
{
RxUartFrameListHead = 0;
RxUartFrameListTail = 0;
TxUartFrameListHead = 0;
TxUartFrameListTail = 0;  
}

///////////////////////////////////////////////////////////////////////////////
//  Tx uart task
//  Jei yra nauju paketu buferyje juos siuncia i UART

void TxUartTask(void)
{
  
}

///////////////////////////////////////////////////////////////////////////////
//  Rx uart task
//
void RxUartTask(void)
{ 
  
u8      temp1;

       // If Receive RS-232 data available

#ifdef RXTX_UART1
  
       if((UART_GetFlagStatus(UART1, UART_FLAG_RxFIFOEmpty) != SET))
       {			
	RxChar = UART_ReceiveData(UART1); 

#else

       if((UART_GetFlagStatus(UART1, UART_FLAG_RxFIFOEmpty) != SET))
       {			
	RxChar = UART_ReceiveData(UART1); 
#endif
        
        switch ( state_incoming )
        {
	
	case INCOMING_STATE_NONE:
			
	  if ( (INCOMING_SUBSTATE_NONE == substate_incoming) && (DLE == RxChar) )
          {
	   substate_incoming = INCOMING_SUBSTATE_DLE;		
	  }
	  else if ( (INCOMING_SUBSTATE_DLE == substate_incoming) && (STX == RxChar) )
          {
           //////// OK paketo pradzia yra
           state_incoming = INCOMING_STATE_STX;		
	   substate_incoming = INCOMING_SUBSTATE_NONE;
	   RxUartFrame.Cnt = 0;
	  }
	  else 
          {
	  state_incoming = INCOMING_STATE_NONE;		
	  substate_incoming = INCOMING_SUBSTATE_NONE;	
	  }
          
       break;								
       case INCOMING_STATE_STX:
          
          if ( (INCOMING_SUBSTATE_NONE == substate_incoming) && (DLE == RxChar) ) 
          {
	   substate_incoming = INCOMING_SUBSTATE_DLE;		
	  }
  	  else if ( (INCOMING_SUBSTATE_DLE == substate_incoming) && (STX == RxChar) )
          { 
          // This is strange as a DEL STX is not expected here
	  // We try to sync up again...
	   state_incoming = INCOMING_STATE_STX;		
       	   substate_incoming = INCOMING_SUBSTATE_NONE;
	   RxUartFrame.Cnt = 0;
	  }
	  else if ( (INCOMING_SUBSTATE_DLE == substate_incoming) && (ETX == RxChar) )
          {	 
          ////////////// Turim teisingo paketo pabaiga /////////////////////
	   state_incoming = INCOMING_STATE_NONE;
	   substate_incoming = INCOMING_SUBSTATE_NONE;
           
              // Pasidedam priimta freima i buferi           
              temp1 = ( RxUartFrameListHead + 1 ) & UART_RX_BUFF_MASK;
              
              if( temp1 != RxUartFrameListTail )
              {
                RxUartFrameList[temp1] = RxUartFrame;
                RxUartFrameListHead = temp1;           
              } 
              else
              {
               Statistics.RxUartOvr++; 
              }
              
//           printf("Turim freima!\r\n");
//           printf("DLC=%d ",RxUartFrame.Cnt);
//           printf("D0=%02X\r\n",RxUartFrame.Data[0]);
                                        
          }
	  else if ( ( INCOMING_SUBSTATE_DLE == substate_incoming ) && (DLE == RxChar) )
          {
	    // Byte stuffed DLE  i.e. DLE DLE == DLE

  	    substate_incoming = INCOMING_SUBSTATE_NONE;	
            
	    if ( RxUartFrame.Cnt <= UART_FRAME_LEN ) 
             {
	      RxUartFrame.Data[ RxUartFrame.Cnt++ ] = RxChar;
	     }
	     else
             {
	      // This packet has wrong format as it have
	      // to many databytes - start over!
              RxUartFrame.Cnt = 0;
	      state_incoming = INCOMING_STATE_NONE;		
	      substate_incoming = INCOMING_SUBSTATE_NONE;	              
	     }          
          }
          else
          {
            substate_incoming = INCOMING_SUBSTATE_NONE;	
            
            if ( RxUartFrame.Cnt <= UART_FRAME_LEN )
            {
	     RxUartFrame.Data[ RxUartFrame.Cnt++ ] = RxChar;
	    }
	    else
            {
	     // This packet has wrong format as it have
	     // to many databytes - start over!
	     state_incoming = INCOMING_STATE_STX;		
	     substate_incoming = INCOMING_SUBSTATE_NONE;	
	    }
          }
	break;	
					
	case INCOMING_STATE_ETX:
	break;		
       }				
      } 
}

///////////////////////////////////////////////////////////////////////////////
//  printf low level func.
//  int MyLowLevelPutchar(int x)
int putchar(int x)  
{
    while(UART_GetFlagStatus(UART1, UART_FLAG_TxFIFOFull) != RESET);
    UART_SendData(UART1,(u8)x);   
    return x;
}

///////////////////////////////////////////////////////////////////////////////
// serPutByteSubst
//

void serPutByteSubst( u8 b )
{
	if ( DLE == b ) {
		putchar((u8) DLE );
		putchar((u8) DLE );	
	}
	else {
		putchar((u8) b );	
	}
}

///////////////////////////////////////////////////////////////////////////////
// Receive Uart frame (from buffer)
// Pasiima paketus is buferio

u8 GetUartFrame(RXTX_UART_FRAME *frame)
{
/*  
       while( RxCanMsgHead != RxCanMsgTail )
     {       
       temp1 = ( RxCanMsgTail + 1 ) & UART_RX_BUFF_MASK;
       RxCanMsgTail = temp1;              
       
       if(RxCanMsg.IdType == CAN_STD_ID)      
         printf("Buffer STD message\r\n");
       else
         printf("Buffer EXT message\r\n");   
     }
*/
  
//  if( !rxframe ) return 0;
  
 return 1; 
}

///////////////////////////////////////////////////////////////////////////////
// Send Uart frame (to buffer)
//
/*
    for(temp = BEGIN_RX_STD_OBJ;temp <= END_RX_STD_OBJ;temp++)
    {     
     if( CAN_ReceiveMessage( temp , FALSE, &RxCanMsg ) == SUCCESS )
     {
      CAN_ReleaseRxMessage( temp );
      
      TempRxCanMsgHead = ( RxCanMsgHead + 1 ) & 0x0F;
      
      if( TempRxCanMsgHead != RxCanMsgTail)
      {
        RxCanMsgList[TempRxCanMsgHead] = RxCanMsg;
        RxCanMsgHead = TempRxCanMsgHead;
        printf("RxCanMsgHead=%d\r\n",RxCanMsgHead);
        printf("RX_STD_OBJ=%d\r\n",temp);     
      }
      else
      {
       printf(" Overhead STD\r\n"); 
      }                        
     }
    }
*/
/*
RXTX_UART_FRAME RxUartFrameList[16];
RXTX_UART_FRAME TxUartFrameList[16];
u8      RxUartFrameListHead;
u8      RxUartFrameTail;
u8      TxUartFrameListHead;
u8      TxUartFrameTail;
*/

////////////////////////////////////////////////////////////////////////////////
//  Deda paketus i buferi
//

u8 PutUartFrame(RXTX_UART_FRAME frame)
{
  u8  temp;        
      
       temp = ( TxUartFrameListHead + 1 ) & UART_TX_BUFF_MASK;           
       if( TxUartFrameListHead != TxUartFrameListHead)
       {
         TxUartFrameList[temp] = frame;
         TxUartFrameListHead = temp;         
       }
       else
       {
        Statistics.TxUartOvr++; 
       }         
       
  return 0;
}
