/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : main.c
 *    Description : main module
 *
 *    History :
 *    1. Date        : August 26, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  This example project shows how to use the IAR Embedded Workbench for ARM
 * to develop code for the IAR STR750-SK evaluation boards. It implements USB
 * CDC (Communication Device Class) device and install it like a Virtual COM
 * port. The UART1 is used for physical implementation of the RS232 port.
 *
 * Jumpers:
 *  PWR_SEL   - depend of power source
 *  VRE       - Absence
 *  BOOT0     - 0
 *  BOOT1     - 0
 *
 *    $Revision: 18265 $
 *
 **************************************************************************/
#include "includes.h"
#include <inttypes.h>       // Set environment with setenv_vscp.bat in vscp folder
#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include "75x_conf.h"
#include "75x_lib.h"
#include "systime.h"
#include "delay.h"
#include "vscp_serial.h"    // Set environment with setenv_vscp.bat in vscp folder
#include "canusb.h"

#define UART  UART_1

// LED Defines
#define GREEN   0
#define RED     1

// buffer for receive messages 
canmsg RxCanMsg;

// Serial output buffer
uint32_t serial_out_pos;
uint8_t serial_out_buf[ SERIAL_OUT_BUFFE_SIZE ];

// CAN output FIFO -- serial to CAN
uint16_t cntOutFrames;
uint16_t posOutFetch;
uint16_t posOutPut;
canmsg outputFrames[ MAX_OUT_FRAMES ];

// CAN input FIFO -- CAN to serial
uint16_t cntInFrames;
uint16_t posInFetch;
uint16_t posInPut;
canmsg inputFrames[ MAX_IN_FRAMES ];

// Sliding Windows on serial channel
uint8_t lastAck;  // Sequence # for last ACK'ed frame

// Statistics
uint32_t cntSentFrames;
uint32_t cntReceivedFrames;
uint32_t cntSentData;
uint32_t cntReceivedData;
uint32_t cntBusWarnings;
uint32_t cntBusOff;
uint32_t cntReceiveOverruns;  // Lost RX frames
uint32_t cntTransmitOverruns; // Lost TX frames

// error states
volatile BOOL bBusOff;
volatile BOOL bBusWarn;
volatile BOOL bErrorPassive;

// blink
uint32_t blink_state_time = 0;
uint8_t blink_state = 0;
uint8_t blink_type = LED_OFF;

// last read CAN traceiver status
volatile uint8_t canstatus;

// Sliding window CAN receive
vs_swFrameStruct swReceiveFrames[ VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES ];
vs_swStruct swReceive;

// used message object numbers 
#define CAN_TX_MSGOBJ       0
#define CAN_RX_MSGOBJ0      1

// CAN Adapter varaible
BOOL CANAdapter_bOpen = FALSE;              // No open
BOOL CANAdapter_bSendErrorFrames = FALSE;   // No error frames as default
uint8_t CANAdapter_modeOpen;
volatile uint8_t CANAdapter_stateChange = TRUE;

// array of pre-defined transmit messages 
canmsg TxCanMsg[3] = {
  { CAN_STD_ID,      0x123, 4, { 0x01, 0x02, 0x04, 0x08 } },
  { CAN_STD_ID,      0x700, 4, { 0xAA, 0x55, 0xAA, 0x55 } },
  { CAN_EXT_ID, 0x12345678, 8, { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 } }
};

// Prototypes
void InitClock ( void );
volatile uint32_t DlyCount;
void InitDlyTimer( uint32_t IntrPriority );
void Dly100us( void *arg );
void IO_Init( void );
void init_CAN( void );
void sendMessage( void );
void CAN_ISR( void );
void ledOn( uint8_t nled );
void LedOff( uint8_t nled );
void setBlinkType( uint8_t type  );
void blink( void );
void sendMessage( void );
Boolean UsbCdcWriteTimeout( uint8_t *Buffer, uint16_t count, uint32_t timeout );

/*************************************************************************
 * Function Name: InitClock
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init MCU clock
 *
 *************************************************************************/
void InitClock ( void )
{
  // Clock
  // MRCC system reset(for debug purpose)
  MRCC_DeInit();
  
  // Wait for OSC4M start-up
  MRCC_WaitForOSC4MStartUp();
  
  // Set HCLK = 64
  MRCC_HCLKConfig( MRCC_CKSYS_Div1 ); // Original div 1  60 MHZ 30
  
  // Set CKTIM = 16 (TB clock etc)
  MRCC_CKTIMConfig( MRCC_HCLK_Div4 ); // Original div 1  60 MHZ 15
  
  // Set PCLK = 8
  MRCC_PCLKConfig( MRCC_CKTIM_Div2 ); // Original div 2  30 MHz 7.5
  
  // Set CKSYS
  MRCC_CKSYSConfig( MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_16 ); // Original 15
  
  // GPIO pins optimized for 3V3 operation
  MRCC_IOVoltageRangeConfig( MRCC_IOVoltageRange_3V3 );
}

volatile uint32_t DlyCount;
/*************************************************************************
 * Function Name: Tim0Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 0 interrupt handler
 *
 *************************************************************************/
void Tim0Handler (void)
{
  if ( DlyCount ) {
    --DlyCount;
  }
  
  // Clear the Update pending Bit
  TIM_ClearITPendingBit( TIM0, TIM_IT_Update );
}

/*************************************************************************
 * Function Name: InitDlyTimer
 * Parameters: Int32U IntrPriority
 *
 * Return: none
 *
 * Description: Init Delay Timer (TIM 0)
 *
 *************************************************************************/
void InitDlyTimer( uint32_t IntrPriority )
{
  MRCC_ClocksTypeDef MRCC_Clocks;
  TIM_InitTypeDef TIM_InitStructure;
  EIC_IRQInitTypeDef EIC_IRQInitStructure;

  // Enable TIM0 clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM0, ENABLE);
  
  // Release TIM0 reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM0,DISABLE);

  // Timer 0
  // TIM Configuration in Output Compare Timing Mode period 100us
  // Calculate counter value
  MRCC_GetClocksStatus(&MRCC_Clocks);

  TIM_InitStructure.TIM_Period = ( MRCC_Clocks.CKTIM_Frequency / 10000 );
  TIM_InitStructure.TIM_Prescaler = 0;
  TIM_InitStructure.TIM_Mode = TIM_Mode_OCTiming;
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_InitStructure.TIM_Channel = TIM_Channel_2;
  TIM_Init( TIM0, &TIM_InitStructure );

  // Enable and configure the priority of the TIM0 Output compare IRQ Channel
  EIC_IRQInitStructure.EIC_IRQChannel = TIM0_UP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrPriority;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit( &EIC_IRQInitStructure );
}

/*************************************************************************
 * Function Name: Dly100us
 * Parameters: void *arg
 *
 * Return: none
 *
 * Description: Delay 100us * arg
 *
 *************************************************************************/
void Dly100us( void *arg )
{
  DlyCount = (uint32_t)arg;

  // Clear TIM0 flags
  TIM_ClearFlag( TIM0, TIM_FLAG_Update );

  // Enable TIM0 Update interrupt
  TIM_ITConfig( TIM0, TIM_IT_Update, ENABLE );

  // Enable TIM0 counter
  TIM_Cmd( TIM0, ENABLE );
  
  while ( DlyCount > 0 );

  // Disable TIM0 Update interrupt
  TIM_ITConfig( TIM0, TIM_IT_Update, DISABLE );

  // Disable TIM0 counter
  TIM_Cmd( TIM0, DISABLE );
}



///////////////////////////////////////////////////////////////////////////////
// IO_Init
//

void IO_Init( void )
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitTypeDef  CAN_RX;
  GPIO_InitTypeDef  CAN_TX;
  
  GPIO_StructInit( &GPIO_InitStructure );
  GPIO_StructInit( &CAN_TX );
  GPIO_StructInit( &CAN_RX );

  // Enable GPIO clocks + CAN clocks
  MRCC_PeripheralClockConfig( MRCC_Peripheral_GPIO | MRCC_Peripheral_CAN, 
                                ENABLE );
  
  // Release GPIO reset
  MRCC_PeripheralSWResetConfig( MRCC_Peripheral_GPIO, DISABLE );

  // IO configure 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_Init( GPIO0, &GPIO_InitStructure );
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  GPIO_Init( GPIO1, &GPIO_InitStructure );
  
  // CAN TX
  CAN_TX.GPIO_Mode = GPIO_Mode_AF_PP;
  CAN_TX.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init( GPIO0, &CAN_TX );

  // CAN RX
  CAN_RX.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  CAN_RX.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init( GPIO0, &CAN_RX );
  
  GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_RESET );  // Turn LED off (Red)
  GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_SET );    // Turn LED on (Green)
  GPIO_WriteBit( GPIO1, GPIO_Pin_0, Bit_RESET );  // USB reset  
}

///////////////////////////////////////////////////////////////////////////////
// ledOn
//

void ledOn( uint8_t nled )
{
  switch ( nled ) {
    case RED:   GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_SET );  
      break;
    case GREEN: GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_SET );  
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////
// ledOff
//

void ledOff( uint8_t nled )
{
  switch ( nled ) {
    case RED:   GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_RESET );  
      break;
    case GREEN: GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_RESET );  
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////
// setBlinkType
//

void setBlinkType( uint8_t type  )
{
  blink_state_time = 0;
  blink_type = type;
  CANAdapter_stateChange = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ledOff
//
// This routine is called from the 1 ms interrupt and blink
// lamp in the selected patters
//


void blink( void ) 
{
  blink_state_time++;
  
  switch( blink_type ) {
  
    case LED_OFF:
      ledOff( RED );
      ledOff( GREEN );
      break;
    
    case LED_RED_STEADY: 
      ledOn( RED );
      ledOff( GREEN );
      break;
    
    case LED_GREEN_STEADY: 
      ledOn( GREEN );
      ledOff( RED );
      break;
    
    case LED_RED_BLINK_FAST:
      ledOff( GREEN );
      if ( 0 == blink_state ) {
        if (  blink_state_time > 500 ) {
          blink_state_time = 0;
          ledOff( RED );
          blink_state = 1;
        }
      }
      else {
        if (  blink_state_time > 500 ) {
          blink_state_time = 0;
          ledOn( RED );
          blink_state = 0;
        }  
      }
      break;
    
    case LED_RED_BLINK_SLOW: 
      ledOff( GREEN );
      if ( 0 == blink_state ) {
        if (  blink_state_time > 1000 ) {
          blink_state_time = 0;
          ledOff( RED );
          blink_state = 1;
        }
      }
      else {
        if (  blink_state_time > 1000 ) {
          blink_state_time = 0;
          ledOn( RED );
          blink_state = 0;
        }  
      }
      break;
    
    case LED_GREEN_BLINK_FAST: 
      ledOff( RED );
      if ( 0 == blink_state ) {
        if (  blink_state_time > 500 ) {
          blink_state_time = 0;
          ledOff( GREEN );
          blink_state = 1;
        }
      }
      else {
        if (  blink_state_time > 500 ) {
          blink_state_time = 0;
          ledOn( GREEN );
          blink_state = 0;
        }  
      }
      break;
    
    case LED_GREEN_BLINK_SLOW: 
      ledOff( RED );
      if ( 0 == blink_state ) {
        if (  blink_state_time > 1000 ) {
          blink_state_time = 0;
          ledOff( GREEN );
          blink_state = 1;
        }
      }
      else {
        if (  blink_state_time > 1000 ) {
          blink_state_time = 0;
          ledOn( GREEN );
          blink_state = 0;
        }  
      }
      break;
    
    case LED_RED_GREEN_BLINK_FAST:
      if ( 0 == blink_state ) {
        if (  blink_state_time > 500 ) {
          blink_state_time = 0;
          ledOff( RED );
          ledOn( GREEN );
          blink_state = 1;
        }
      }
      else {
        if (  blink_state_time > 500 ) {
          blink_state_time = 0;
          ledOff( GREEN );
          ledOn( RED );
          blink_state = 0;
        }  
      }
      break;
      
    case LED_RED_GREEN_BLINK_SLOW:
      if ( 0 == blink_state ) {
        if (  blink_state_time > 1000 ) {
          blink_state_time = 0;
          ledOff( RED );
          ledOn( GREEN );
          blink_state = 1;
        }
      }
      else {
        if (  blink_state_time > 1000 ) {
          blink_state_time = 0;
          ledOff( GREEN );
          ledOn( RED );
          blink_state = 0;
        }  
      }
      break;
      
  }
}

///////////////////////////////////////////////////////////////////////////////
// CAN_Com_Polling
//

void CAN_Com_Polling( void )
{
/*  
  CAN_InitTypeDef CAN_Struct;
  
  CAN_EnterInitMode( CAN_CR_CCE ); 
  
  // initialize the CAN at a standard bitrate, interrupts disabled 
  CAN_Struct.CAN_ConfigParameters=0x0;
  CAN_Struct.CAN_Bitrate=CAN_BITRATE_500K;
  CAN_Init( &CAN_Struct );
 
  // configure the message objects  
  CAN_InvalidateAllMsgObj();
  CAN_SetTxMsgObj( CAN_TX_STD_MSGOBJ, CAN_STD_ID );
  CAN_SetRxMsgObj( CAN_RX_MSGOBJ0, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE );
  CAN_LeaveInitMode( );
*/
  // Send the pre-defined answer
  CAN_SendMessage( CAN_TX_MSGOBJ, &TxCanMsg[ 0 ] );

  // wait until end of transmission 
  CAN_WaitEndOfTx();
  
  CAN_ReleaseTxMessage( CAN_TX_MSGOBJ );

/* 
  // wait for reception of a data frame 
  while ( !CAN_ReceiveMessage( CAN_RX_MSGOBJ, FALSE, &RxCanMsg ) )
  {
    // Add Timer
  }

  // Test Received Msg 
  if ( ( RxCanMsg.IdType == CAN_STD_ID )&&
      ( RxCanMsg.Id == 0x321 ) && 
      ( RxCanMsg.Dlc == 4 ) &&
      ( RxCanMsg.Data[0] == 0xAA ) &&
      ( RxCanMsg.Data[1] == 0x55 ) &&
      ( RxCanMsg.Data[2] == 0xAA ) && 
      ( RxCanMsg.Data[3] == 0x55 ) ) {
    // Received Msg OK
    LED_ON( GREEN );
  } 
  else {
    // Received Msg KO
    LED_OFF( RED );
  }
*/

  // release the message objects 
  //CAN_ReleaseTxMessage( CAN_TX_STD_MSGOBJ );
  //CAN_ReleaseRxMessage( CAN_RX_MSGOBJ0 );
  
  delay_ms( 10 );
}

///////////////////////////////////////////////////////////////////////////////
// CAN_Com_IRQ
//

void CAN_Com_IRQ( void )
{
  EIC_IRQInitTypeDef  EIC_IRQInitStructure;
  CAN_InitTypeDef     CAN_Struct;
  
  // initialize the interrupt controller 
  EIC_DeInit();
    
  EIC_IRQInitStructure.EIC_IRQChannel=CAN_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority=1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
	
  // initialize the CAN at a standard bitrate, interrupts enabled 
  CAN_Struct.CAN_ConfigParameters=CAN_CR_IE;
  CAN_Struct.CAN_Bitrate=CAN_BITRATE_500K;
  CAN_Init(&CAN_Struct);

  // configure the message objects 
  CAN_EnterInitMode( CAN_CR_CCE );
  CAN_InvalidateAllMsgObj();
  CAN_SetTxMsgObj( CAN_TX_MSGOBJ, CAN_EXT_ID );
  CAN_SetRxMsgObj( CAN_RX_MSGOBJ0, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE );
  CAN_LeaveInitMode();
  
  // enable global interrupt 
  EIC_IRQCmd( ENABLE );

  // Send the pre-defined frame 
  CAN_SendMessage( CAN_TX_MSGOBJ, &TxCanMsg[2] );

  // wait until end of transmission 
  CAN_WaitEndOfTx();
  
  // reception and release are done in the interrupt handler 

  // delay to add when the reception occurs 
  Int32U dly = 0xffff;
  Dly100us( &dly );

  // Test Received Msg 
  if ( ( RxCanMsg.IdType == CAN_EXT_ID ) && 
      ( RxCanMsg.Id == 0x12345678 ) &&
      ( RxCanMsg.Dlc == 8 ) &&
      ( RxCanMsg.Data[0] == 0x10 ) &&
      ( RxCanMsg.Data[1] == 0x11 ) &&
      ( RxCanMsg.Data[2] == 0x12 ) && 
      ( RxCanMsg.Data[3] == 0x13 ) &&
      ( RxCanMsg.Data[4] == 0x14 ) &&
      ( RxCanMsg.Data[5] == 0x15 ) &&
      ( RxCanMsg.Data[6] == 0x16 ) &&
      ( RxCanMsg.Data[7]==0x17 ) ) {
    // Received Msg OK
  } else {
    // Received Msg KO

  }

  // disable interrupts globally 
  EIC_IRQCmd( DISABLE );
}

///////////////////////////////////////////////////////////////////////////////
// init_CAN
//

void init_CAN( void )
{
  int i;
  
  EIC_IRQInitTypeDef  EIC_IRQInitStructure;
  CAN_InitTypeDef     CAN_Struct;
  
  // Init output fifo
  cntOutFrames = 0;
  posOutFetch = 0;
  posOutPut = 0;
  
  // Init input fifo
  cntInFrames = 0;
  posInFetch = 0;
  posInPut = 0;
  
  // Last ACK'ed frame is none
  lastAck = 0;
  
  // initialize the interrupt controller 
  EIC_IRQInitStructure.EIC_IRQChannel = CAN_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit( &EIC_IRQInitStructure );
  
  CAN_EnterInitMode( CAN_CR_CCE );
   
  // initialize the CAN at a standard bitrate, interrupts enabled 
  CAN_Struct.CAN_ConfigParameters = CAN_CR_IE | CAN_CR_EIE | CAN_CR_SIE;
  CAN_Struct.CAN_Bitrate = CAN_BITRATE_500K;
  CAN_Init( &CAN_Struct );
  
  // configure the message objects 
  CAN_InvalidateAllMsgObj();
  
  // Configure the standard id transmit object
  CAN_SetTxMsgObj( CAN_TX_MSGOBJ, CAN_STD_ID );
  
  // Configure the extended id transmit object
  CAN_SetTxMsgObj( CAN_TX_MSGOBJ + 1, CAN_EXT_ID );
  
  // Configure receive objects
  for ( i=2; i<15; i++ ) {
    CAN_SetRxMsgObj( i, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE );
  }
  for ( i=16; i<32; i++ ) {
    CAN_SetRxMsgObj( i, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE );
  }
  
  CAN_LeaveInitMode();
  
  // enable global interrupt 
  EIC_IRQCmd( ENABLE );
}

///////////////////////////////////////////////////////////////////////////////
// sendMessage
//

void sendMessage( void )
{
  canmsg msg;
  vs_frame *pFrame;
  
  if ( cntOutFrames ) {           // If anything to send
        
    pFrame = (vs_frame *)&outputFrames[ posOutFetch ];
      
    // Send the message
    if ( !( pFrame->data[0] & 0x80 ) ) {
      // Standard id
      msg.IdType = 0;
      if ( pFrame->data[0] & 0x40 ) {
        msg.IdType |= 0x00;   // Remote frame
      }
      
      msg.Id = pFrame->data[3] + 
                ( pFrame->data[2] << 8 ) +
                ( pFrame->data[1] << 16 ) +
                ( ( pFrame->data[0] & 0x0f ) << 24 );
      msg.Dlc = ( pFrame->flags & 0x1f ) - 8;
      memcpy( msg.Data, ( pFrame->data + 4 ), msg.Dlc );
      CAN_SendMessage( CAN_TX_MSGOBJ, &msg );
    }
    else {
      // Extended id
      msg.IdType = CAN_EXT_ID;
      if ( pFrame->data[0] & 0x40 ) {
        msg.IdType |= 0x00;   // Remote frame
      }
      
      msg.Id = pFrame->data[ 3 ] + 
                ( pFrame->data[ 2 ] << 8 ) +
                ( pFrame->data[ 1 ] << 16 ) +
                ( ( pFrame->data[ 0 ] & 0x0f ) << 24 );
      msg.Dlc = ( pFrame->flags & 0x1f ) - 4;
      memcpy( msg.Data, ( pFrame->data + 8 ), msg.Dlc );
      CAN_SendMessage( CAN_TX_MSGOBJ + 1, &msg );
    }
    
    posOutFetch++;
    if ( posOutFetch >= MAX_OUT_FRAMES ) posOutFetch = 0;
    cntOutFrames--;
        
    // Statistics
    cntSentFrames++;
    cntSentData += outputFrames[ posOutFetch ].Dlc;
  }
}

///////////////////////////////////////////////////////////////////////////////
// CAN_ISR
//
// CAN interrupt service routine
//

void CAN_ISR( void )
{
  canmsg RxCanMsg;
  uint8_t msgobj;
  uint16_t idr;
  
  if ( 0x8000 == ( idr = CAN->IDR ) ) {
    
    // We have a status interrupt
    // bit 7 - Bus off
    // bit 6 - Warning
    // bit 5 - Error Passive
    // bit 4 - RXOK
    // bit 3 - TXOK
    // bit 2-0 - Last Error Code.
    canstatus = CAN->SR;
    
    // May need visual indication
    CANAdapter_stateChange = TRUE;
    
    // Check for bus off
    if ( bBusOff ) {
      if ( !( canstatus & CAN_SR_BOFF ) ) {
        // Going away from bus off
        bBusOff = FALSE;  
      }
    }
    else {
      if ( canstatus & CAN_SR_BOFF ) {
        // bus off
        bBusOff = TRUE;
      }  
    }
    
    // Check for bus warning
    if ( bBusWarn ) {
      if ( !( canstatus & CAN_SR_EWARN ) ) {
        // Going away from bus warning
        bBusWarn = FALSE;  
      }
    }
    else {
      if ( canstatus & CAN_SR_EWARN ) {
        // bus warning
        bBusWarn = TRUE;
      }  
    }
    
    // Check for error passive
    if ( bErrorPassive ) {
      if ( !( canstatus & CAN_SR_EPASS ) ) {
        // Going away from error passive
        bErrorPassive = FALSE;  
      }
    }
    else {
      if ( canstatus & CAN_SR_EPASS ) {
        // error passive
        bErrorPassive = TRUE;  
      }  
    }
    
  }
  else if ( msgobj = ( idr & 0x1f ) ) {
    
    if ( msgobj < 3 ) {
      
      // We have transmitted a message, send next if one is availabe    
      
      // Clear interrupt pending bit
      CAN->sMsgObj[1].CMR = CAN_CMR_CLRINTPND;
      CAN->sMsgObj[1].CRR = msgobj;
      
      sendMessage();
      
    }
    else {
      
      // We have received a message - get it
      CAN_ReceiveMessage( msgobj-1, TRUE, &RxCanMsg );
      
      if ( CANAdapter_bOpen ) { // Receive only if open
        // Statistics
        cntReceivedFrames++;
        cntReceivedData += RxCanMsg.Dlc;
      
        if ( cntInFrames <= MAX_IN_FRAMES ) {
          memcpy( &inputFrames[ posInPut ], &RxCanMsg, sizeof( RxCanMsg ) );
          cntInFrames++;
          posInPut++;
          if ( posInPut >= MAX_IN_FRAMES ) posInPut = 0;
        }
        else {
          cntReceiveOverruns++;
        }
        
      }
      
    }
       
  }
}



/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description:
 *
 *************************************************************************/

int main( void )
{
  uint16_t i;
  uint8_t Buffer[ 1024 ];
  uint32_t readCnt; // Incoming data count
  uint32_t readPos; // Serial incoming data pointer
  Boolean CdcConfigureStateHold;
  
  // Initialize output fifo 
  cntOutFrames = 0;
  posOutFetch = 0;
  posOutPut = 0;
  
  // Initialize input fifo 
  cntInFrames = 0;
  posInFetch = 0;
  posInPut = 0;
  
  // Initialize statistics
  cntSentFrames = 0;
  cntReceivedFrames = 0;
  cntSentData = 0;
  cntReceivedData = 0;
  cntBusWarnings = 0;
  cntBusOff = 0;
  cntReceiveOverruns = 0;
  cntTransmitOverruns = 0;
  
  // Error states
  bBusOff = FALSE;
  bBusWarn = FALSE;
  bErrorPassive = FALSE;
  
  // Sliding window init for USB receive
  swReceive.cntOutstanding = 0;
  swReceive.maxtime = 500;
  swReceive.maxresend = 3;
  
  // Application
  vs_frame serialFrame;       // VSCP Serial frame
  canmsg msg;                 // CAN frame
  uint16_t com_channel = 0;   // Communication channel (Always zero)
  uint8_t seqnumber = 0;      // Packet sequence number
  
  // Set Burst mode
  CFG_FLASHBurstConfig( CFG_FLASHBurst_Enable );
  
  // MCU clock init
  InitClock();
  
  // EIC init
  EIC_DeInit();
  
  // Delay timer init
  InitDlyTimer( 1 );
  
  // Init 1 ms timebase
  initTimebase();
  
  // Init UART 0/1
  //UartInit( UART, 3 );

  // GPIO ports init
  IO_Init();
  
  // Initialize USB
  // Init USB
  USB_Init( 5, 4, UsbCdcConfigure );
  
  // Init CDC
  UsbCdcInit( 2 );
  
  // Init CAN interface
  init_CAN();
  
  // Soft connection enable
  USB_ConnectRes( TRUE );

  // Enable the Interrupt controller to manage IRQ channel
  EIC_IRQCmd( ENABLE );
  __enable_interrupt();

  CdcConfigureStateHold = !UsbCdcIsCdcConfigure();

  setBlinkType( LED_RED_BLINK_FAST );
  
  
  while( 1 ) {
    
    if ( UsbCdcIsCdcConfigure() ) {
      
      if ( CANAdapter_stateChange ) {
        if ( CANAdapter_bOpen  ) {
          if ( bBusOff ) {
            setBlinkType( LED_RED_BLINK_FAST ); 
          }
          else if ( bBusWarn ) {
            setBlinkType( LED_RED_GREEN_BLINK_FAST );
          }
          else {
            setBlinkType( LED_GREEN_BLINK_FAST );
          }
        }
        else {
          setBlinkType( LED_GREEN_STEADY );
        }
        
        CANAdapter_stateChange = FALSE;
        
      }
      
      
      if ( CdcConfigureStateHold == FALSE ) {
        CdcConfigureStateHold = TRUE;
      }
      
      // Data from USB
      if ( readCnt = UsbCdcRead( Buffer, sizeof( Buffer ) ) ) {
        
        readPos = 0;
        do {
       
          if ( vs_feed( Buffer[ readPos ] ) ) {
            
            // We have a frame
            if ( vs_getFrame( &serialFrame ) ) {
            
              switch ( serialFrame.opcode ) {
            
                case VSCP_SERIAL_OPCODE_NOOP:
                  
                  // We don't do anything other then ACK the frame
                  vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
                  break;
              
                case VSCP_SERIAL_OPCODE_LEVEL1_EVENT:
                  // We don't accept LEVEL I events
                  vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
                  break;
                  
                case VSCP_SERIAL_OPCODE_CAN_FRAME:
                  
                  if ( CANAdapter_bOpen ) { // Send only if open
                    // Place message in outqueue if there is room for it
                    if ( cntOutFrames > MAX_OUT_FRAMES ) {
                      // No room for frame - TX overrun
                      cntTransmitOverruns++;
                    }
                    else {
                    
                      // A CAN frame has it's id in the four first
                      // bytes of the data area and timestamp in the
                      // last four databytes. This means that
                      // the data count must be at least eight bytes
                     // for a fram to be a valid CAN frame.
                      // id + data + timestamp
                      if ( ( serialFrame.flags & 0x1f ) >= 8 ) {
                         
                        // Place frame in outqueue
                        lastAck = serialFrame.seqnumber;
                        memcpy( &outputFrames[ posOutPut++ ], 
                                  &serialFrame, 
                                  sizeof( serialFrame ) );
                        cntOutFrames++;
                        posOutPut %= MAX_OUT_FRAMES;
                           
                        // If the CAN output queue was empty we need to send
                        // the event here
                        if ( 0 == ( cntOutFrames - 1 ) ) {
                          sendMessage();
                        }
                      
                      }
                    
                    }
                  }
                  else {
                    // Not open
                    vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
                  }
                  break;
              
                case VSCP_SERIAL_OPCODE_LEVEL2_EVENT:
                  // We don't accept LEVEL II events
                  vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
                  break;
              
                case VSCP_SERIAL_OPCODE_POLL_EVENT:
                  // We don't accept poll events
                  vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
                  break;
              
                case VSCP_SERIAL_OPCODE_NO_EVENT:
                  // We don't do anything
                  break;
              
                case VSCP_SERIAL_OPCODE_ACK:
                  // ACK received - remove the frame for the active frames
                  break;
              
                case VSCP_SERIAL_OPCODE_NACK: 
                  // NACK received
                  break;
              
                case VSCP_SERIAL_OPCODE_ERROR:
                  // We don't do anything
                  break;
                  
                case VSCP_SERIAL_OPCODE_REPLY:
                  // We don't do anything
                  break;    
              
                case VSCP_SERIAL_OPCODE_COMMAND:
                  switch ( serialFrame.vscptype ) {
                    
                    case CANUSB_COMMAND_NOOP:
                      // OK
                      vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
                      break;
                     
                    case CANUSB_COMMAND_SET_BITRATE:
                      
                      CAN_EnterInitMode( CAN_CR_CCE | CAN_CR_IE | CAN_CR_EIE | CAN_CR_SIE );
                      
                      if ( 0 == serialFrame.data[ 0 ] ) {
                                      
                        // Standard baudrate
                        switch ( serialFrame.data[ 1 ] ) {
                        
                          case 0x00:      
                            CAN_SetBitrate( CAN_BITRATE_10K );
                            break;
                            
                          case 0x01:      
                            CAN_SetBitrate( CAN_BITRATE_20K );
                            break;                            
                            
                          case 0x02:      
                            CAN_SetBitrate( CAN_BITRATE_50K );
                            break;                            
                            
                          case 0x03:      
                            CAN_SetBitrate( CAN_BITRATE_100K );
                            break;                            
                            
                          case 0x04:      
                            CAN_SetBitrate( CAN_BITRATE_125K );
                            break;                            
                          
                          case 0x05:      
                            CAN_SetBitrate( CAN_BITRATE_250K );
                            break;                            
                            
                          case 0x06:      
                            CAN_SetBitrate( CAN_BITRATE_500K );
                            break;                            
                            
                          case 0x07:      
                            CAN_SetBitrate( CAN_BITRATE_800K );
                            break;                            
                            
                          case 0x08:      
                            CAN_SetBitrate( CAN_BITRATE_1M );
                            break;                            
                        }
                        
                      }
                      else {
                        // Custom bitrate settings
                        CAN->BTR = 256 * serialFrame.data[ 1 ] + serialFrame.data[ 2 ];    
                        CAN->BRPR = serialFrame.data[ 3 ];
                      }
                      
                      CAN_LeaveInitMode();
                      CAN_LeaveTestMode();
                      vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
                      break;
                    
                    case CANUSB_COMMAND_SET_FILTER:
                      vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
                      break;
                    
                    case CANUSB_COMMAND_SET_MASK:
                      vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
                      break;
                    
                    case CANUSB_COMMAND_OPEN:
                      setBlinkType( LED_GREEN_BLINK_FAST  );
                      CANAdapter_bOpen = TRUE;
                      vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
                      break;
                    
                    case CANUSB_COMMAND_CLOSE:
                      setBlinkType( LED_GREEN_STEADY );
                      CANAdapter_bOpen = FALSE;
                      vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
                      break;
                    
                    case CANUSB_COMMAND_GET_STATUS:
                      
                      break;
                    
                    case CANUSB_COMMAND_GET_VERSION:
                      
                      break;
                    
                    case CANUSB_COMMAND_GET_SERIALNO:
                      
                      break;
                    
                    case CANUSB_COMMAND_FIND_BITRATE:
                      
                      break;
                    
                    case CANUSB_COMMAND_GET_STAT_FRAME:
                      
                      break;
                    
                    case CANUSB_COMMAND_GET_STAT_DATA:
                      
                      break;
                    
                    case CANUSB_COMMAND_GET_STAT_ERROR:
                      
                      break;
                      
                    case CANUSB_COMMAND_CLR_STAT:
                      
                      break;                      

                    default:
                      // Unknown command
                      vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
                      break;
                  }
                  break;
                  
              } // frame type
              
            } // get frame
            
          } // new frame
          
          // Next Character.
          readPos++;
          
        } while( --readCnt );
        
      } // Data available

      // Send frames to USB host if any pending
      if ( CANAdapter_bOpen && ( cntInFrames > 0 ) ) { // Only if open
        
        memcpy( &msg, &inputFrames[ posInFetch ], sizeof( canmsg )  );
        
        serialFrame.channel = com_channel;
	serialFrame.seqnumber = seqnumber++;
	serialFrame.flags = 4 + 4 + msg.Dlc; // id + timestamp + dlc
	serialFrame.data[ 3 ] = (uint8_t)(( msg.Id >> 0 ) & 0xff);
	serialFrame.data[ 2 ] = (uint8_t)(( msg.Id >> 9 ) & 0xff);
	serialFrame.data[ 1 ] = (uint8_t)(( msg.Id >> 16 ) & 0xff);
	serialFrame.data[ 0 ] = (uint8_t)(( msg.Id >> 24 ) & 0xff);
        if ( CAN_EXT_ID ==  msg.IdType ) {
	  serialFrame.data[ 0 ] |= VSCP_SERIAL_CAN_DATA0_EXTENDED;
        }
     
        
        // Data
	for ( i=0; i<msg.Dlc; i++ ) {
	  serialFrame.data[ 4 + i ] = msg.Data[ i ];	
	}
        
        // Timestamp  
        serialFrame.data[ 7 + msg.Dlc ] = (uint8_t)(( getSysTICs() >> 0 ) & 0xff);
	serialFrame.data[ 6 + msg.Dlc ] = (uint8_t)(( getSysTICs() >> 9 ) & 0xff);
	serialFrame.data[ 5 + msg.Dlc ] = (uint8_t)(( getSysTICs() >> 16 ) & 0xff);
	serialFrame.data[ 4 + msg.Dlc ] = (uint8_t)(( getSysTICs() >> 24 ) & 0xff);
        
	serialFrame.opcode = VSCP_SERIAL_OPCODE_CAN_FRAME;
	serialFrame.vscpclass = 0;
	serialFrame.vscptype = 0;
	
	vs_sendFrame( &serialFrame );
        
        posInFetch++;
        if ( posInFetch >= MAX_IN_FRAMES ) posInFetch = 0;
        cntInFrames--;
        
      }
      

      // Data from UART
      //readCnt = UartRead( UART, Buffer, sizeof( Buffer ) - 1 );
      //if ( readCnt ) {
      //  while(!UsbCdcWrite( Buffer, readCnt ) );
      //}
      //else {
      //  strcpy( Buffer, "Hello World\n" );
      //  UsbCdcWriteTimeout( Buffer, 100 );
      //}
    } // USB configured
    else {
      if ( TRUE == CdcConfigureStateHold ) {
        CdcConfigureStateHold = FALSE;
      }
    }
  } // Main loop
}

///////////////////////////////////////////////////////////////////////////////
// UsbCdcWriteTimeout
//

Boolean UsbCdcWriteTimeout( uint8_t *Buffer, uint16_t count, uint32_t timeout ) 
{
  uint32_t tics = getSysTICs();
  
  while ( getElapsedSysTICs( tics ) < timeout ) {
    if ( UsbCdcWrite( Buffer, count ) ) return TRUE;   
  }
  
  return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// vs_sendBuffer
//
// This routine is required by the VSCP serial protocol. 
//

BOOL vs_sendBuffer( uint8_t *pbuf, uint16_t count  )
{
  // UsbCdcWrite( pbuf, count );
  //while ( !UsbCdcWrite( pbuf, count ) );
  if ( !UsbCdcWriteTimeout(  pbuf, count, 500 ) ) {
    return FALSE;  
  }
    
  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// vs_sendSerialByte
//
// This routine is required by the VSCP serial protocol. 
//

vs_sendSerialByte( uint8_t b )
{
  if ( !UsbCdcWriteTimeout( &b, 1, 500 ) ) {
    return FALSE;  
  }
    
  return TRUE;
}