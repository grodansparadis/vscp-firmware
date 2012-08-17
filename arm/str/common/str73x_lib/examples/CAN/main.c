/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : CAN Main program Example
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
/* Include of other module interface headers -----------------------------------*/
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/ 


/* Define GPIO Init Structure */

GPIO_InitTypeDef CAN0_RX;
GPIO_InitTypeDef CAN0_TX;
GPIO_InitTypeDef CAN1_RX;
GPIO_InitTypeDef CAN1_TX;
GPIO_InitTypeDef CAN2_RX;
GPIO_InitTypeDef CAN2_TX;
GPIO_InitTypeDef Led_Config;
GPIO_InitTypeDef GPIO4_InitStructure;

/*define de LEDs*/
#define LD2	GPIO_PIN_0	
#define LD1	GPIO_PIN_1	
#define LD5	GPIO_PIN_2	
#define LD3	GPIO_PIN_3	
#define LD6	GPIO_PIN_4	
#define LD4	GPIO_PIN_5	
#define LD10	GPIO_PIN_6	
#define LD8	GPIO_PIN_7	
#define LD9	GPIO_PIN_8	
#define LD7	GPIO_PIN_9	
#define LD13	GPIO_PIN_10	
#define LD11	GPIO_PIN_11	
#define LD14	GPIO_PIN_12	
#define LD12	GPIO_PIN_13	
#define LD16	GPIO_PIN_14	
#define LD15	GPIO_PIN_15	

// buffer for receive messages
canmsg RxCan0Msg;

// array of pre-defined transmit messages
canmsg TxCan0Msg[2] = {
	{ CAN_STD_ID,      0x123, 4, { 0x01, 0x02, 0x04, 0x08 } },
	{ CAN_EXT_ID, 0x12345678, 8, { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 } }
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


/* Functions Prototypes-------------------------------------------------------*/

void wait(void)
{
	u32 i;
	for (i=100000L; i!=0 ; i-- ) {}
}

void CAN0_Demo_Polling(void)
{

CAN_InitTypeDef CANInit = {0x0,CAN_BITRATE_1M};


	// initialize the CAN at a standard bitrate, interrupts disabled
	CAN_Init(CAN0, &CANInit);

	// switch into Loopback+Silent mode (self-test)
	CAN_EnterTestMode(CAN0, CAN_TESTR_LBACK | 0);


	// configure the message objects
	CAN_InvalidateAllMsgObj(CAN0);
	CAN_SetTxMsgObj(CAN0, CAN0_TX_MSGOBJ, CAN_STD_ID);
	CAN_SetRxMsgObj(CAN0, CAN0_RX_MSGOBJ, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE);


	// send a pre-defined data frame
	(void)CAN_SendMessage(CAN0, CAN0_TX_MSGOBJ, &TxCan0Msg[0]);

	// wait until end of transmission
	CAN_WaitEndOfTx(CAN0);

	GPIO_BitWrite(GPIO4,LD1,Bit_SET);//LED1;

	// wait for reception of a data frame
	while (!CAN_ReceiveMessage(CAN0, CAN0_RX_MSGOBJ, FALSE, &RxCan0Msg))
	{
		// add a time-out handling here, if necessary
	}

	
        //Test Received Msg
        if((RxCan0Msg.IdType == CAN_STD_ID)&&(RxCan0Msg.Id == 0x123)&&(RxCan0Msg.Dlc == 4)
           &&(RxCan0Msg.Data[0]==0x01)&&(RxCan0Msg.Data[1]==0x02)&&(RxCan0Msg.Data[2]==0x04)&&(RxCan0Msg.Data[3]==0x08)){
          //Received Msg OK
	GPIO_BitWrite(GPIO4,LD2,Bit_SET);//LED2;
        } else {
          //Received Msg KO
        GPIO_BitWrite(GPIO4,LD1,Bit_RESET);//LED1 off;
        }

	// release the message objects
	CAN_ReleaseTxMessage(CAN0, CAN0_TX_MSGOBJ);
	CAN_ReleaseRxMessage(CAN0, CAN0_RX_MSGOBJ);

	// switch back into Normal mode
	CAN_LeaveTestMode(CAN0);
}

void CAN1_Demo_Polling(void)
{

CAN_InitTypeDef CAN1Init = {0x0,CAN_BITRATE_100K};


	// initialize the CAN at a standard bitrate, interrupts disabled
	CAN_Init(CAN1, &CAN1Init);
	
	// switch into Loopback+Silent mode (self-test)
	CAN_EnterTestMode(CAN1, CAN_TESTR_LBACK | 0);


	// configure the message objects
	CAN_InvalidateAllMsgObj(CAN1);
	CAN_SetTxMsgObj(CAN1, CAN1_TX_MSGOBJ, CAN_STD_ID);
	CAN_SetRxMsgObj(CAN1, CAN1_RX_MSGOBJ, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE);

	// send a pre-defined data frame
	(void)CAN_SendMessage(CAN1, CAN1_TX_MSGOBJ, &TxCan1Msg[0]);

	// wait until end of transmission
	CAN_WaitEndOfTx(CAN1);

	GPIO_BitWrite(GPIO4,LD3,Bit_SET);//LED3;

	// wait for reception of a data frame
	while (!CAN_ReceiveMessage(CAN1, CAN1_RX_MSGOBJ, FALSE, &RxCan1Msg))
	{
		// add a time-out handling here, if necessary
	}

	
        //Test Received Msg
        if((RxCan1Msg.IdType == CAN_STD_ID)&&(RxCan1Msg.Id == 0x123)&&(RxCan1Msg.Dlc == 4)
           &&(RxCan1Msg.Data[0]==0x01)&&(RxCan1Msg.Data[1]==0x02)&&(RxCan1Msg.Data[2]==0x04)&&(RxCan1Msg.Data[3]==0x08)){
          //Received Msg OK
	GPIO_BitWrite(GPIO4,LD5,Bit_SET);//  LED5;
        } else {
          //Received Msg KO
        GPIO_BitWrite(GPIO4,LD3,Bit_RESET);//LED3 off;
        }

	// release the message objects
	CAN_ReleaseTxMessage(CAN1, CAN1_TX_MSGOBJ);
	CAN_ReleaseRxMessage(CAN1, CAN1_RX_MSGOBJ);


	// switch back into Normal mode
	CAN_LeaveTestMode(CAN1);
}

void CAN2_Demo_Polling(void)
{

CAN_InitTypeDef CANInit;
CANInit.CAN_Mask = 0x0;
CANInit.CAN_Bitrate = CAN_BITRATE_500K;


	// initialize the CAN at a standard bitrate, interrupts disabled
	CAN_Init(CAN2, &CANInit);

	// switch into Loopback+Silent mode (self-test)
	CAN_EnterTestMode(CAN2, CAN_TESTR_LBACK | CAN_TESTR_SILENT);


	// configure the message objects
	CAN_InvalidateAllMsgObj(CAN2);
	CAN_SetTxMsgObj(CAN2, CAN2_TX_MSGOBJ, CAN_STD_ID);
	CAN_SetRxMsgObj(CAN2, CAN2_RX_MSGOBJ, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE);

	// send a pre-defined data frame
	(void)CAN_SendMessage(CAN2, CAN2_TX_MSGOBJ, &TxCan2Msg[0]);

	// wait until end of transmission
	CAN_WaitEndOfTx(CAN2);

	GPIO_BitWrite(GPIO4,LD4,Bit_SET);//LED4;

	// wait for reception of a data frame
	while (!CAN_ReceiveMessage(CAN2, CAN2_RX_MSGOBJ, FALSE, &RxCan2Msg))
	{
		// add a time-out handling here, if necessary
	}

        //Test Received Msg
        if((RxCan2Msg.IdType == CAN_STD_ID)&&(RxCan2Msg.Id == 0x123)&&(RxCan2Msg.Dlc == 4)
           &&(RxCan2Msg.Data[0]==0x01)&&(RxCan2Msg.Data[1]==0x02)&&(RxCan2Msg.Data[2]==0x04)&&(RxCan2Msg.Data[3]==0x08)){
          //Received Msg OK
	GPIO_BitWrite(GPIO4,LD6,Bit_SET);//LED6;
        } else {
          //Received Msg KO
        GPIO_BitWrite(GPIO4,LD4,Bit_RESET);//LED4 off;
        }

	// release the message objects
	CAN_ReleaseTxMessage(CAN2, CAN2_TX_MSGOBJ);
	CAN_ReleaseRxMessage(CAN2, CAN2_RX_MSGOBJ);

	// switch back into Normal mode
	CAN_LeaveTestMode(CAN2);
}

void CAN0_Demo_Interrupt(void)
{
CAN_InitTypeDef CAN0Init;
CAN0Init.CAN_Mask = CAN_CR_IE;
CAN0Init.CAN_Bitrate = CAN_BITRATE_100K;

	// initialize the interrupt controller
	EIC_IRQChannelConfig(CAN0_IRQChannel, ENABLE);
	EIC_IRQChannelPriorityConfig(CAN0_IRQChannel, 1);
	EIC_IRQCmd(ENABLE);

	// initialize the CAN at a standard bitrate, interrupts enabled
	CAN_Init(CAN0, &CAN0Init);

	// switch into Loopback+Silent mode (self-test)
	CAN_EnterTestMode(CAN0, CAN_TESTR_LBACK | CAN_TESTR_SILENT);

	// configure the message objects
	CAN_InvalidateAllMsgObj(CAN0);
	CAN_SetTxMsgObj(CAN0, CAN0_TX_MSGOBJ, CAN_EXT_ID);
	CAN_SetRxMsgObj(CAN0, CAN0_RX_MSGOBJ, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE);

	// send a pre-defined data frame
	(void)CAN_SendMessage(CAN0, CAN0_TX_MSGOBJ, &TxCan0Msg[1]);

	// reception and release are done in the interrupt handler

	wait();

	GPIO_BitWrite(GPIO4,LD10,Bit_SET);//LED10

	// switch back into Normal mode
	CAN_LeaveTestMode(CAN0);

	// disable interrupts globally
	EIC_IRQCmd(DISABLE);
}

void CAN1_Demo_Interrupt(void)
{
CAN_InitTypeDef CAN1Init;
CAN1Init.CAN_Mask = CAN_CR_IE;
CAN1Init.CAN_Bitrate = CAN_BITRATE_500K;

	// initialize the interrupt controller
	EIC_IRQChannelConfig(CAN1_IRQChannel, ENABLE);
	EIC_IRQChannelPriorityConfig(CAN1_IRQChannel, 2);
	EIC_IRQCmd(ENABLE);

	// initialize the CAN at a standard bitrate, interrupts enabled
	CAN_Init(CAN1, &CAN1Init);

	// switch into Loopback+Silent mode (self-test)
	CAN_EnterTestMode(CAN1, CAN_TESTR_LBACK | CAN_TESTR_SILENT);

	// configure the message objects
	CAN_InvalidateAllMsgObj(CAN1);
	CAN_SetTxMsgObj(CAN1, CAN1_TX_MSGOBJ, CAN_EXT_ID);
	CAN_SetRxMsgObj(CAN1, CAN1_RX_MSGOBJ, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE);

	
	// send a pre-defined data frame
	(void)CAN_SendMessage(CAN1, CAN1_TX_MSGOBJ, &TxCan1Msg[1]);

	// reception and release are done in the interrupt handler

	wait();

	GPIO_BitWrite(GPIO4,LD9,Bit_SET);//LED9


	// switch back into Normal mode
	CAN_LeaveTestMode(CAN1);

	// disable interrupts globally
	EIC_IRQCmd(DISABLE);
}
void CAN2_Demo_Interrupt(void)
{
CAN_InitTypeDef CAN2Init;
CAN2Init.CAN_Mask = CAN_CR_IE;
CAN2Init.CAN_Bitrate = CAN_BITRATE_125K;

	// initialize the interrupt controller
	EIC_IRQChannelConfig(CAN2_IRQChannel, ENABLE);
	EIC_IRQChannelPriorityConfig(CAN2_IRQChannel, 1);
	EIC_IRQCmd(ENABLE);

	// initialize the CAN at a standard bitrate, interrupts enabled
	CAN_Init(CAN2, &CAN2Init);

	// switch into Loopback+Silent mode (self-test)
	CAN_EnterTestMode(CAN2, CAN_TESTR_LBACK | CAN_TESTR_SILENT);

	// configure the message objects
	CAN_InvalidateAllMsgObj(CAN2);
	CAN_SetTxMsgObj(CAN2, CAN2_TX_MSGOBJ, CAN_EXT_ID);
	CAN_SetRxMsgObj(CAN2, CAN2_RX_MSGOBJ, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE);

	// send a pre-defined data frame
	(void)CAN_SendMessage(CAN2, CAN2_TX_MSGOBJ, &TxCan2Msg[1]);

	// reception and release are done in the interrupt handler

	wait();

	GPIO_BitWrite(GPIO4,LD13,Bit_SET);//LED13

	// switch back into Normal mode
	CAN_LeaveTestMode(CAN2);

	// disable interrupts globally
	EIC_IRQCmd(DISABLE);
}

void MCLK_Config (void)
{

PRCCU_InitTypeDef Clock;
CMU_InitTypeDef ClockInit;

Clock.PRCCU_DIV2=DISABLE;
Clock.PRCCU_MCLKSRC_SRC=PRCCU_MCLKSRC_PLL;
Clock.PRCCU_PLLDIV=PRCCU_PLLMUL_12;
Clock.PRCCU_PLLMUL=PRCCU_PLLDIV_6;

ClockInit.CMU_RCOscControl=0x0;
ClockInit.CMU_EndCountValue=0x0;
ClockInit.CMU_FreqRef_High=0x0;
ClockInit.CMU_FreqRef_Low=0x0;
ClockInit.CMU_CKSEL0=0x0;
ClockInit.CMU_CKSEL1=0x0;
ClockInit.CMU_CKSEL2=CMU_CKSEL0_CKOSC;


CMU_Lock (DISABLE);
CMU_Init (&ClockInit);


PRCCU_Init(&Clock);
}


/*---------------------------------main---------------------------------------*/

void main()
{

#ifdef DEBUG
    debug();
#endif

	CAN0_RX.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
	CAN0_RX.GPIO_Pins = GPIO_PIN_14;
	CAN0_TX.GPIO_Mode = GPIO_Mode_AF_PP;
	CAN0_TX.GPIO_Pins = GPIO_PIN_15;
	CAN1_RX.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
	CAN1_RX.GPIO_Pins = GPIO_PIN_1;
	CAN1_TX.GPIO_Mode = GPIO_Mode_AF_PP;
	CAN1_TX.GPIO_Pins = GPIO_PIN_2;
	CAN2_RX.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
	CAN2_RX.GPIO_Pins = GPIO_PIN_5;
	CAN2_TX.GPIO_Mode = GPIO_Mode_AF_PP;
	CAN2_TX.GPIO_Pins = GPIO_PIN_4;

	Led_Config.GPIO_Mode = GPIO_Mode_OUT_PP;
	Led_Config.GPIO_Pins = 0xFFFF;//GPIO_PIN_ALL;


	MCLK_Config();	
 
        CFG_PeripheralClockConfig (CFG_CLK_CAN0, ENABLE);
        CFG_PeripheralClockConfig (CFG_CLK_CAN1, ENABLE);
        CFG_PeripheralClockConfig (CFG_CLK_CAN2, ENABLE);
        CFG_PeripheralClockConfig (CFG_CLK_GPIO1, ENABLE);
        CFG_PeripheralClockConfig (CFG_CLK_GPIO2, ENABLE);
        CFG_PeripheralClockConfig (CFG_CLK_GPIO4, ENABLE);
        CFG_PeripheralClockConfig (CFG_CLK_GPIO6, ENABLE);
        CFG_PeripheralClockConfig (CFG_CLK_EIC, ENABLE);

	GPIO_Init(GPIO4, &Led_Config);
	GPIO_Init(GPIO1, &CAN0_RX);
	GPIO_Init(GPIO1, &CAN0_TX);
	GPIO_Init(GPIO2, &CAN1_RX);
	GPIO_Init(GPIO2, &CAN1_TX);


	GPIO_WordWrite(GPIO4,0x0000);//Leds off

	while(1)
		{
		CAN0_Demo_Polling();
		CAN1_Demo_Polling();
		CAN2_Demo_Polling();
		CAN0_Demo_Interrupt();
		CAN1_Demo_Interrupt();
		CAN2_Demo_Interrupt();
		//12 LEDs should lit
		}

}//main

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
