/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Main program body
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

/* Standard include ------------------------------------------------------------*/
#include "73x_lib.h"
#include "../../common/73x_lcd.h"
#include "../../common/systime.h"
#include "../../common/delay.h"

// Prototypes
void wait( void );
void MCLK_Config (void);
void CAN0_Demo_Polling( void );
void CAN0_Demo_Interrupt(void);
unsigned int get1000Digit( unsigned int val );
unsigned int get100Digit( unsigned int val );
unsigned int get10Digit( unsigned int val );
unsigned int get1Digit( unsigned int val );

// Peripherals Init Structures
GPIO_InitTypeDef GPIO0_InitStructure;
GPIO_InitTypeDef GPIO3_InitStructure;
ADC_InitTypeDef  ADC_InitStructure;
CMU_InitTypeDef  CMU_InitStructure;
RTC_InitTypeDef  RTC_InitStructure;

GPIO_InitTypeDef CAN0_RX;
GPIO_InitTypeDef CAN0_TX;
GPIO_InitTypeDef CAN1_RX;
GPIO_InitTypeDef CAN1_TX;
GPIO_InitTypeDef CAN2_RX;
GPIO_InitTypeDef CAN2_TX;
GPIO_InitTypeDef Led_Config;

// define the LEDs
#define LD1		GPIO_PIN_0	
#define LD2		GPIO_PIN_1	
#define LD3		GPIO_PIN_2	
#define LD4		GPIO_PIN_3	
#define LD5		GPIO_PIN_4	
#define LD6		GPIO_PIN_5	
#define LD7		GPIO_PIN_6	
#define LD8		GPIO_PIN_7	
#define LD9		GPIO_PIN_8	
#define LD10	GPIO_PIN_9	
#define LD11	GPIO_PIN_10	
#define LD12	GPIO_PIN_11	
#define LD13	GPIO_PIN_12	
#define LD14	GPIO_PIN_13	
#define LD15	GPIO_PIN_14	
#define LD16	GPIO_PIN_15	

// buffer for receive messages
canmsg RxCan0Msg;

// array of pre-defined transmit messages
canmsg TxCan0Msg[ 2 ] = {
	{ CAN_STD_ID,
                  0x123,
                  4,
                  { 0x01, 0x02, 0x04, 0x08 } },
	{ CAN_EXT_ID,
                  0x12345678,
                  8,
                  { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 } }
};

// buffer for receive messages
canmsg RxCan1Msg;

// array of pre-defined transmit messages
canmsg TxCan1Msg[2] = {
	{ CAN_STD_ID,      0x123, 4, { 0x01, 0x02, 0x04, 0x08 } },
	{ CAN_EXT_ID, 0x12345678, 8, { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 } }
};

// buffer for receive messages
canmsg RxCan2Msg;

// array of pre-defined transmit messages
canmsg TxCan2Msg[2] = {
	{ CAN_STD_ID,      0x123, 4, { 0x01, 0x02, 0x04, 0x08 } },
	{ CAN_EXT_ID, 0x12345678, 8, { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 } }
};

// used message object numbers
enum {
	CAN0_TX_MSGOBJ = 0,
	CAN0_RX_MSGOBJ = 1
};
enum {
	CAN1_TX_MSGOBJ = 0,
	CAN1_RX_MSGOBJ = 1
};
enum {
	CAN2_TX_MSGOBJ = 0,
	CAN2_RX_MSGOBJ = 1
};

u16 Conversion_Value = 0;
u8 buffer[11] = "ADC: ";



/* Private function prototypes -------------------------------------------------*/

/* Interface functions ---------------------------------------------------------*/

/* Private functions -----------------------------------------------------------*/


int main(void)
{
	u32 CPIO_Value;
	
#ifdef LIBDEBUG
	libdebug();
#endif

	MCLK_Config();	
	
	// Enable clock on both CAN channels
	CFG_PeripheralClockConfig( CFG_CLK_CAN0, ENABLE );
  CFG_PeripheralClockConfig( CFG_CLK_CAN1, ENABLE );

  CFG_PeripheralClockConfig( CFG_CLK_GPIO0, ENABLE );
  CFG_PeripheralClockConfig( CFG_CLK_GPIO1, ENABLE );
  CFG_PeripheralClockConfig( CFG_CLK_GPIO2, ENABLE );
  CFG_PeripheralClockConfig( CFG_CLK_GPIO6, ENABLE );

  Led_Config.GPIO_Mode = GPIO_Mode_OUT_PP;
  Led_Config.GPIO_Pins = 0xFFFF;
  GPIO_Init( GPIO0, &Led_Config );
	
	GPIO3_InitStructure.GPIO_Mode = GPIO_Mode_HI_AIN_TRI;
  GPIO3_InitStructure.GPIO_Pins = GPIO_PIN_ALL;
  GPIO_Init( GPIO3, &GPIO3_InitStructure );

  CAN0_RX.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
  CAN0_RX.GPIO_Pins = GPIO_PIN_14;
  GPIO_Init( GPIO1, &CAN0_RX );

  CAN0_TX.GPIO_Mode = GPIO_Mode_AF_PP;
  CAN0_TX.GPIO_Pins = GPIO_PIN_15;
  GPIO_Init( GPIO1, &CAN0_TX );

  CAN1_RX.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
  CAN1_RX.GPIO_Pins = GPIO_PIN_1;
  GPIO_Init( GPIO2, &CAN1_RX );

  CAN1_TX.GPIO_Mode = GPIO_Mode_AF_PP;
  CAN1_TX.GPIO_Pins = GPIO_PIN_2;
  GPIO_Init( GPIO2, &CAN1_TX );

  // Init 1 ms timebase
  initTimebase2();

  GPIO_WordWrite( GPIO0, 0x0000 );  // All LED's off

  // * * * EIC * * *

  // EIC Clock Enable
  CFG_PeripheralClockConfig( CFG_CLK_EIC , ENABLE );

  // EIC Config
  EIC_IRQChannelPriorityConfig( RTC_IRQChannel, 1 );
  EIC_IRQChannelPriorityConfig( TB2_IRQChannel, 2 );

  //EIC_IRQChannelConfig( RTC_IRQChannel , ENABLE );
  

  EIC_IRQCmd( ENABLE );
	
	// * * * CMU * * *
	
  // CMU Initialization 
  CMU_DeInit( );
  CMU_StructInit(&CMU_InitStructure);
	
  // Enable the external oscillator 
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;
  CMU_Init(&CMU_InitStructure);
	
  // Disable DIV2 
  PRCCU->CFR &= 0x7FFF;


  // * * * RTC * * *

  // Set RTC clock-------
  // (clock:0-->9) fOSC=4Mhz/RTCP=(2,4,8,16,32,64,128,256,512,1024)
  //            -->EXTCLK=(4M,2M,1M,500K,250K,125K,62.5K,31.25K,15.625K,7.812K)


  // RTC Clock Enable
  CFG_PeripheralClockConfig( CFG_CLK_RTC , ENABLE );

  // RTC Interrupt Config
  RTC_ITConfig( RTC_IT_SEC | RTC_IT_ALA | RTC_IT_OV, ENABLE );

  // RTC Configuration
  RTC_InitStructure.RTC_Alarm = 6; 		// 	0xFFFFFFFF;
  RTC_InitStructure.RTC_Counter = 0; 	//	0xFFFFFFFA;
  // configure RTC=1S  ( RCK=1000000/64=CKL32=15625Hz )
  // 8MHz/64 = 125000 = 0x1E848
  RTC_InitStructure.RTC_Prescaler = 0x3D09; // == 15625
  //RTC_InitStructure.RTC_Prescaler = 0x1E848; // 0x3D09; // == 15625

  RTC_Init( &RTC_InitStructure );



  // * * * CAN * * *
  CAN_InitTypeDef CANInit = { 0x0, CAN_BITRATE_500K };

  // initialize the CAN at a standard bitrate, interrupts disabled
  CAN_Init( CAN0, &CANInit );

  // configure the message objects
  CAN_EnterInitMode( CAN0, CAN_CR_CCE );
  CAN_InvalidateAllMsgObj( CAN0 );
  CAN_SetTxMsgObj( CAN0, CAN0_TX_MSGOBJ, CAN_STD_ID );
  CAN_SetRxMsgObj( CAN0, CAN0_RX_MSGOBJ, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE );
	CAN_LeaveInitMode( CAN0 );
	
	GPIO_BitWrite( GPIO0, LD1, Bit_SET );
	
  // * * * LCD * * *
	
  // LCD Configuration
  LCD_Init();
	LCD_Clear();
  //LCD_LineClear(1);
  //LCD_LineClear(2);
  LCD_SetPosCur(1,1);

  // Display text message on LCD: Line 1
  LCD_SendString( "SIMPLE CAN TEST", 1, 1 );
  LCD_SetPosCur(2,1);

	
	// * * * ADC * * *
	
	
  // ADC Clock Enable 
  CFG_PeripheralClockConfig(CFG_CLK_ADC, ENABLE);
	
  // ADC Configuration 
  ADC_DeInit ();
  ADC_StructInit( &ADC_InitStructure );
  ADC_InitStructure.ADC_Calibration = ADC_Calibration_ON;
  ADC_InitStructure.ADC_CalibAverage = ADC_CalibAverage_Enable;
  ADC_InitStructure.ADC_AutoClockOff = ADC_AutoClockOff_Disable ;
  ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_Scan ;
  ADC_InitStructure.ADC_SamplingPrescaler = 0x2;
  ADC_InitStructure.ADC_ConversionPrescaler = 0x4;
  ADC_InitStructure.ADC_FirstChannel = ADC_CHANNEL0 ;
  ADC_InitStructure.ADC_ChannelNumber = 1;
  ADC_Init( &ADC_InitStructure );


  // EOC interrupt Enable 
  ADC_ITConfig( ADC_IT_EOC, ENABLE );
	
  // Enable ADC 
  ADC_Cmd( ENABLE );
	
	

	while ( 1 )	 {
	
		// send a pre-defined data frame
    CAN_SendMessage( CAN0, CAN0_TX_MSGOBJ, &TxCan0Msg[0] );

    // wait until end of transmission
    CAN_WaitEndOfTx( CAN0 );

    // release the TX message object
    CAN_ReleaseTxMessage( CAN0, CAN0_TX_MSGOBJ );

    // Check for a data frame
    if ( CAN_ReceiveMessage( CAN0, CAN0_RX_MSGOBJ, FALSE, &RxCan0Msg ) ) {
	
      // Test Received Msg
      if ( ( RxCan0Msg.IdType == CAN_STD_ID ) &&
            ( RxCan0Msg.Id == 0x123) &&
            ( RxCan0Msg.Dlc == 4 ) &&
            ( RxCan0Msg.Data[ 0 ] == 0x01 ) &&
            ( RxCan0Msg.Data[ 1 ] == 0x02 ) &&
            ( RxCan0Msg.Data[ 2 ] == 0x04 ) &&
            ( RxCan0Msg.Data[ 3 ] == 0x08 ) ){

        // Received Msg OK
        GPIO_BitWrite( GPIO0,LD15, Bit_SET );   // LED 15
				
				LCD_SendString( "RECEIVE CAN MSG", 2, 1 );

      }
      else {

        // Received some other Msg
        GPIO_BitWrite( GPIO0, LD14, Bit_SET ); // LED 14
				
				LCD_LineClear(2);

      }

      // Release the RX message object
      CAN_ReleaseRxMessage(CAN0, CAN0_RX_MSGOBJ);

    } // Received frame
		
	
		// Start ADC conversion 
		ADC_ConversionCmd( ADC_ConversionStart );
		buffer[8] = get1Digit( Conversion_Value );
		buffer[7] = get10Digit( Conversion_Value );
		buffer[6] = get100Digit( Conversion_Value );
		buffer[5] = get1000Digit( Conversion_Value );
		buffer[9] = 0;
		
		// Display ADC conversion Value on LCD 
		//LCD_SendString( &buffer[5], 2, 5 );
		CPIO_Value = 0;

		for(u32 i = (u32)((float)Conversion_Value/60); i; i--) {
			CPIO_Value <<= 1;
			CPIO_Value |= 1;
		}
		
		GPIO_WordWrite( GPIO0, CPIO_Value );

		// Wait for 500 ms
		delay_ms( 500 );

	} // while
	
}

////////////////////////////////////////////////////////////////////////////////
// get1000Digit
//

unsigned int get1000Digit( unsigned int val )
{
  return ( ( val / 1000 ) + 0x30 );
}

////////////////////////////////////////////////////////////////////////////////
// get100Digit
//

unsigned int get100Digit( unsigned int val )
{
  unsigned int valCopy;
  valCopy = val % 1000;
  return (  (valCopy / 100 ) + 0x30 );
}

////////////////////////////////////////////////////////////////////////////////
// get10Digit
//

unsigned int get10Digit( unsigned int val )
{
  unsigned int valCopy;
  valCopy = val % 100;
  return ( ( valCopy / 10 ) + 0x30 );
}

////////////////////////////////////////////////////////////////////////////////
// get1Digit
//

unsigned int get1Digit( unsigned int val )
{
  unsigned int valCopy;
  valCopy = val % 100;
  return ( ( valCopy % 10 ) + 0x30 );
}

////////////////////////////////////////////////////////////////////////////////
//  wait
//

void wait( void )
{
  u32 i;

  for (i=100000L; i!=0 ; i-- ) {}
}

////////////////////////////////////////////////////////////////////////////////
//  MCLK_Config
//

void MCLK_Config (void)
{

  PRCCU_InitTypeDef Clock;
  CMU_InitTypeDef ClockInit;

  ClockInit.CMU_RCOscControl = 0x0D;  // RC OSC Adjust
  ClockInit.CMU_EndCountValue = 0x0F; //
  ClockInit.CMU_FreqRef_High = 0xFF;  //
  ClockInit.CMU_FreqRef_Low = 0xFE;   //

  ClockInit.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;  // Clock for CLOCK1 (4MHz)
  ClockInit.CMU_CKSEL1 = CMU_CKSEL1_CKPLL;  // Clock for Freq. Meter (8MHz)
  ClockInit.CMU_CKSEL2 = CMU_CKSEL2_CKOSC;  // Clock to drive CMU logic

  CMU_Lock( DISABLE );
  CMU_Init( &ClockInit );

   // Init. Power Reset Clock unit
  //    4 MHz * 12 / 6  = 8 MHz = CLOCK2
  Clock.PRCCU_DIV2 = DISABLE;
  Clock.PRCCU_MCLKSRC_SRC = PRCCU_MCLKSRC_PLL;  // Use PLL in
  Clock.PRCCU_PLLDIV = PRCCU_PLLDIV_6;
  Clock.PRCCU_PLLMUL = PRCCU_PLLMUL_12;

  PRCCU_Init( &Clock );
  PRCCU_SetExtClkDiv(4);

}

////////////////////////////////////////////////////////////////////////////////
//  CAN0_Demo_Polling
//

void CAN0_Demo_Polling( void )
{
  CAN_InitTypeDef CANInit = { 0x0, CAN_BITRATE_500K };

  // initialize the CAN at a standard bitrate, interrupts disabled
  CAN_Init( CAN0, &CANInit );

  // switch into Loopback+Silent mode (self-test)
  //CAN_EnterTestMode( CAN0, CAN_TESTR_LBACK | 0 );

  // configure the message objects
  CAN_InvalidateAllMsgObj( CAN0 );
  CAN_SetTxMsgObj( CAN0, CAN0_TX_MSGOBJ, CAN_STD_ID );
  CAN_SetRxMsgObj( CAN0, CAN0_RX_MSGOBJ, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE );

  // send a pre-defined data frame
  CAN_SendMessage( CAN0, CAN0_TX_MSGOBJ, &TxCan0Msg[0] );

  // wait until end of transmission
  CAN_WaitEndOfTx( CAN0 );

  GPIO_BitWrite( GPIO0, LD16, Bit_SET );   // LED16;

/*
  // wait for reception of a data frame
  while ( !CAN_ReceiveMessage( CAN0, CAN0_RX_MSGOBJ, FALSE, &RxCan0Msg ) ) {
    // add a time-out handling here, if necessary
  }
	
  // Test Received Msg
  if ( ( RxCan0Msg.IdType == CAN_STD_ID ) &&
          ( RxCan0Msg.Id == 0x123) &&
          ( RxCan0Msg.Dlc == 4 ) &&
          ( RxCan0Msg.Data[ 0 ] == 0x01 ) &&
          ( RxCan0Msg.Data[ 1 ] == 0x02 ) &&
          ( RxCan0Msg.Data[ 2 ] == 0x04 ) &&
          ( RxCan0Msg.Data[ 3 ] == 0x08 ) ){

    // Received Msg OK
    GPIO_BitWrite(GPIO0,LD1,Bit_SET);//LED1;
  }
  else {

    // Received Msg OK
    GPIO_BitWrite(GPIO0,LD2,Bit_RESET);//LED2 off;

  }
*/
  // release the message objects
  CAN_ReleaseTxMessage(CAN0, CAN0_TX_MSGOBJ);
  CAN_ReleaseRxMessage(CAN0, CAN0_RX_MSGOBJ);

  // switch back into Normal mode
  //CAN_LeaveTestMode( CAN0 );

}

////////////////////////////////////////////////////////////////////////////////
//  CAN0_Demo_Interrupt
//

void CAN0_Demo_Interrupt(void)
{
  CAN_InitTypeDef CAN0Init;
  CAN0Init.CAN_Mask = CAN_CR_IE;
  CAN0Init.CAN_Bitrate = CAN_BITRATE_100K;

  // initialize the interrupt controller
  EIC_IRQChannelConfig( CAN0_IRQChannel, ENABLE );
  EIC_IRQChannelPriorityConfig( CAN0_IRQChannel, 1 );
  EIC_IRQCmd( ENABLE );

  // initialize the CAN at a standard bitrate, interrupts enabled
  CAN_Init( CAN0, &CAN0Init );

  // switch into Loopback+Silent mode (self-test)
  CAN_EnterTestMode( CAN0, CAN_TESTR_LBACK | CAN_TESTR_SILENT );

  // configure the message objects
  CAN_InvalidateAllMsgObj( CAN0 );
  CAN_SetTxMsgObj( CAN0, CAN0_TX_MSGOBJ, CAN_EXT_ID );
  CAN_SetRxMsgObj( CAN0, CAN0_RX_MSGOBJ, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE );

  // send a pre-defined data frame
  CAN_SendMessage( CAN0, CAN0_TX_MSGOBJ, &TxCan0Msg[1] );

  // reception and release are done in the interrupt handler
  wait();

  GPIO_BitWrite(GPIO0,LD7,Bit_SET); // LED7

  // switch back into Normal mode
  CAN_LeaveTestMode( CAN0 );

  // disable interrupts globally
  EIC_IRQCmd( DISABLE );

}   



/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

























































































