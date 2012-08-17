/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team & 
//                    Ake Hedman, eurosource, <akhe@eurosource.se>
* Date First Issued  : 03/10/2006
* Description        : Main program body
********************************************************************************
* History:
* 02/28/2008 : Adopted for real life and USBCAN adapterkarin 
* 07/17/2006 : V1.0
* 03/10/2006 : V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "75x_lib.h"

GPIO_InitTypeDef    GPIO_InitStructure;
GPIO_InitTypeDef    CAN_RX;
GPIO_InitTypeDef    CAN_TX;
EIC_IRQInitTypeDef  EIC_IRQInitStructure;
CAN_InitTypeDef     CAN_Struct;
ErrorStatus OSC4MStartUpStatus;

// LED Defines
#define GREEN   0
#define RED     1

// buffer for receive messages 
canmsg RxCanMsg;

// used message object numbers 
enum {
  CAN_TX_MSGOBJ = 0,
  CAN_RX_MSGOBJ = 1
};

// array of pre-defined transmit messages 
canmsg TxCanMsg[3] = {
  { CAN_STD_ID,      0x123, 4, { 0x01, 0x02, 0x04, 0x08 } },
  { CAN_STD_ID,      0x721, 4, { 0xAA, 0x55, 0xAA, 0x55 } },
  { CAN_EXT_ID, 0x12345678, 8, { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 } }
};


///////////////////////////////////////////////////////////////////////////////
// delay
//
  
void delay(u32 XTime)
{
  u32 j;

  for ( j=0; j<XTime; j++ );
}

///////////////////////////////////////////////////////////////////////////////
// LED_ON
//

void LED_ON( u8 LED )
{
  switch ( LED ) {
    case RED:   GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_SET );  break;
    case GREEN: GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_SET );  break;
    default:;
  }
}

///////////////////////////////////////////////////////////////////////////////
// LED_OFF
//

void LED_OFF( u8 LED )
{
  switch ( LED ) {
    case RED:   GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_RESET );  break;
    case GREEN: GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_RESET );  break;
    default:;
  }
}


///////////////////////////////////////////////////////////////////////////////
// InitClock
//

void InitClock( void )
{
  // Clock ( 8 MHz Chrystal )
  
  // MRCC system reset(for debug purpose)
  MRCC_DeInit();
    
  // Wait for OSC4M start-up
  OSC4MStartUpStatus = MRCC_WaitForOSC4MStartUp();
  
  if ( SUCCESS == OSC4MStartUpStatus ) {
      
    // Set HCLK (Configures the AHB clock) 32MHZ
    MRCC_HCLKConfig( MRCC_CKSYS_Div2 );
  
    // Set CKTIM (Configures the TIM clock) 16MHZ
    MRCC_CKTIMConfig( MRCC_HCLK_Div2 );
  
    // Set PCLK (Configures the APB clock) 8MHz
    MRCC_PCLKConfig( MRCC_CKTIM_Div2 );
  
    // Set CKSYS to 64 MHz
    MRCC_CKSYSConfig( MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_16 ); // 64 MHz

  }
  
  // GPIO pins optimized for 3V3 operation
   MRCC_IOVoltageRangeConfig( MRCC_IOVoltageRange_3V3 );
}

///////////////////////////////////////////////////////////////////////////////
// System_Setup
//

void System_Setup(void)
{
  InitClock();
  
  // GPIO and CAN clock enabling 
  MRCC_PeripheralClockConfig( MRCC_Peripheral_GPIO | MRCC_Peripheral_CAN, 
                                ENABLE );
}

///////////////////////////////////////////////////////////////////////////////
// IO_Init
//

void IO_Init(void)
{
  GPIO_DeInit( GPIO0 );
  GPIO_DeInit( GPIO1 );

  /* set CAN TX and CAN RX */
  GPIO_StructInit( &GPIO_InitStructure );
  GPIO_StructInit( &CAN_TX );
  GPIO_StructInit( &CAN_RX );
  
  // CAN TX
  CAN_TX.GPIO_Mode = GPIO_Mode_AF_PP;
  CAN_TX.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init( GPIO0, &CAN_TX );

  // CAN RX
  CAN_RX.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  CAN_RX.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init( GPIO0, &CAN_RX );
  
  // LED
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO0, &GPIO_InitStructure);
  
  LED_ON( GREEN );
}



///////////////////////////////////////////////////////////////////////////////
// CAN_Com_Polling
//

void CAN_Com_Polling(void)
{
  CAN_EnterInitMode( CAN_CR_CCE ); 
  
  // initialize the CAN at a standard bitrate, interrupts disabled 
  CAN_Struct.CAN_ConfigParameters=0x0;
  CAN_Struct.CAN_Bitrate=CAN_BITRATE_500K;
  CAN_Init( &CAN_Struct );

  // configure the message objects  
  CAN_InvalidateAllMsgObj();
  CAN_SetTxMsgObj( CAN_TX_MSGOBJ, CAN_STD_ID );
  CAN_SetRxMsgObj( CAN_RX_MSGOBJ, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE );
  CAN_LeaveInitMode( );

  // Send the pre-defined answer 
  CAN_SendMessage( CAN_TX_MSGOBJ, &TxCanMsg[1] );

  // wait until end of transmission 
  CAN_WaitEndOfTx();
  
  CAN_ReleaseTxMessage( CAN_TX_MSGOBJ );
  
  // message sent 
  //LED_ON(LD2);

  
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

  // release the message objects 
  CAN_ReleaseTxMessage( CAN_TX_MSGOBJ );
  CAN_ReleaseRxMessage( CAN_RX_MSGOBJ );


}

///////////////////////////////////////////////////////////////////////////////
// CAN_Com_IRQ
//

void CAN_Com_IRQ(void)
{
  // initialize the interrupt controller 
  EIC_DeInit();
  EIC_IRQInitStructure.EIC_IRQChannel=CAN_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority=1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd=ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
	

  // initialize the CAN at a standard bitrate, interrupts enabled 
  CAN_Struct.CAN_ConfigParameters=CAN_CR_IE;
  CAN_Struct.CAN_Bitrate=CAN_BITRATE_125K;
  CAN_Init(&CAN_Struct);

  // configure the message objects 
  CAN_EnterInitMode( CAN_CR_CCE );
  CAN_InvalidateAllMsgObj();
  CAN_SetTxMsgObj(CAN_TX_MSGOBJ, CAN_EXT_ID);
  CAN_SetRxMsgObj(CAN_RX_MSGOBJ, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE);
  CAN_LeaveInitMode();
  
  // enable global interrupt 
  EIC_IRQCmd(ENABLE);

  // Send the pre-defined frame 
  CAN_SendMessage(CAN_TX_MSGOBJ, &TxCanMsg[2]);

  // wait until end of transmission 
  CAN_WaitEndOfTx();
  
  // Message Sent 
  LED_ON( RED );

  // reception and release are done in the interrupt handler 

  // delay to add when the reception occurs 
  delay( 0xFFFF );

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
    LED_ON( GREEN );
  } else {
    // Received Msg KO
    LED_OFF( RED );
  }

  // disable interrupts globally 
  EIC_IRQCmd( DISABLE );
}

///////////////////////////////////////////////////////////////////////////////
// main
//

int main()
{
  #ifdef DEBUG
    debug();
  #endif

  // System configuration 
  System_Setup();   // set CAN input clock at 8MHz 
  IO_Init();
  
  while ( 1 ) {
    
    // CAN tests 
    CAN_Com_Polling();    
    CAN_Com_IRQ();

  }
  
}
