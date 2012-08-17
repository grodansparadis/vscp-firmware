/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_can.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file contains all the functions prototypes for the
*                      CAN bus software library.
**********************************************************************************
* History:
* 12/20/2004 :  V1.1
* 08/02/2004 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

#ifndef __720_CAN_H
#define __720_CAN_H

#include "720_lib.h"

#ifndef _WIU
  #error _WIU Must be Defined in "720_conf.h"
#endif

// Standard bitrates available
enum
{
	CAN_BITRATE_100K,
	CAN_BITRATE_125K,
	CAN_BITRATE_250K,
	CAN_BITRATE_500K,
	CAN_BITRATE_1M
};

// Control register
#define CAN_CTL_TEST		0x0080
#define CAN_CTL_CCE		0x0040
#define CAN_CTL_DAR		0x0020
#define CAN_CTL_EIE		0x0008
#define CAN_CTL_SIE		0x0004
#define CAN_CTL_IE		0x0002
#define CAN_CTL_INIT		0x0001

// Status register
#define CAN_STS_LEC		0x0007
#define CAN_STS_TXOK		0x0008
#define CAN_STS_RXOK		0x0010
#define CAN_STS_EPASS		0x0020
#define CAN_STS_EWARN		0x0040
#define CAN_STS_BOFF		0x0080

// Test register
#define CAN_TST_RX		0x0080
#define CAN_TST_TX1		0x0040
#define CAN_TST_TX0		0x0020
#define CAN_TST_LBACK		0x0010
#define CAN_TST_SILENT		0x0008
#define CAN_TST_BASIC		0x0004

// IFx / Command Request register
#define CAN_CRQ_BUSY		0x8000

// IFx / Command Mask register
#define CAN_COM_WRITE		0x0080
#define CAN_COM_MASK		0x0040
#define CAN_COM_ARB		0x0020
#define CAN_COM_CTRL		0x0010
#define CAN_COM_CLRINT		0x0008
#define CAN_COM_TXRQST		0x0004
#define CAN_COM_DATAA		0x0002
#define CAN_COM_DATAB		0x0001

// IFx / Mask1,2 register
#define CAN_MSK_MXTD		0x8000
#define CAN_MSK_MDIR		0x4000

// IFx / Arbitration1,2 register
#define CAN_ARB_MSGVAL		0x8000
#define CAN_ARB_XTD		0x4000
#define CAN_ARB_DIR		0x2000

// IFx / Message Control register
#define CAN_CTL_NEWDAT		0x8000
#define CAN_CTL_MSGLST		0x4000
#define CAN_CTL_INTPND		0x2000
#define CAN_CTL_UMASK		0x1000
#define CAN_CTL_TXIE		0x0800
#define CAN_CTL_RXIE		0x0400
#define CAN_CTL_RMTEN		0x0200
#define CAN_CTL_TXRQST		0x0100
#define CAN_CTL_EOB		0x0080


// Wake-up modes
enum
{
	CAN_WAKEUP_ON_EXT,
	CAN_WAKEUP_ON_CAN
};


// CAN message structure
typedef struct
{
	int IdType;
	vu32 Id;
	vu8 Dlc;
	vu8 Data[8];
} canmsg;

// message ID types
enum
{
	CAN_STD_ID,
	CAN_EXT_ID
};

// message ID limits
#define CAN_LAST_STD_ID	((1<<11) - 1)
#define CAN_LAST_EXT_ID	((1L<<29) - 1)

/*******************************************************************************
* Function Name  : CAN_EnterInitMode
* Description    : Switch the CAN into initialization mode
* Input 1        : any binary value formed from the CAN_CTL_xxx defines
* Output         : None
* Return         : None
* Note           : CAN_LeaveInitMode must be called when all is done
*******************************************************************************/
static inline void CAN_EnterInitMode(vu8 mask)
{
	CAN->CR = mask | CAN_CTL_INIT;
	CAN->SR = 0;					// reset the status
}

/*******************************************************************************
* Function Name  : CAN_LeaveInitMode
* Description    : Leave the initialization mode (switch into normal mode)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static inline void CAN_LeaveInitMode(void)
{
	CAN->CR &= ~(CAN_CTL_INIT | CAN_CTL_CCE);
}

/*******************************************************************************
* Function Name  : CAN_EnterTestMode
* Description    : Switch the CAN into test mode
* Input 1        : any binary value formed from the CAN_TST_xxx defines
* Output         : None
* Return         : None
* Note           : CAN_LeaveTestMode must be called when all is done
*******************************************************************************/
static inline void CAN_EnterTestMode(vu8 mask)
{
	CAN->CR |= CAN_CTL_TEST;
	CAN->TESTR |= mask;
}

/*******************************************************************************
* Function Name  : CAN_LeaveTestMode
* Description    : Leave the current test mode (switch into normal mode)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static inline void CAN_LeaveTestMode(void)
{
	CAN->CR |= CAN_CTL_TEST;
	CAN->TESTR &= ~(CAN_TST_LBACK | CAN_TST_SILENT | CAN_TST_BASIC);
	CAN->CR &= ~CAN_CTL_TEST;
}

/*******************************************************************************
* Function Name  : CAN_SetBitrate
* Description    : Setup a standard CAN bitrate
* Input 1        : one of the CAN_BITRATE_xxx defines
* Output         : None
* Return         : None
* Note           : CAN must be in initialization mode
*******************************************************************************/
void CAN_SetBitrate(int bitrate);

/*******************************************************************************
* Function Name  : CAN_SetTiming
* Description    : Setup the CAN timing with specific parameters
* Input 1        : Time Segment before the sample point position, from 1 to 16
* Input 2        : Time Segment after the sample point position, from 1 to 8
* Input 3        : Synchronisation Jump Width, from 1 to 4
* Input 4        : Baud Rate Prescaler, from 1 to 1024
* Output         : None
* Return         : None
* Note           : CAN must be in initialization mode
*******************************************************************************/
void CAN_SetTiming(int tseg1, int tseg2, int sjw, int brp);

/*******************************************************************************
* Function Name  : CAN_SleepRequest
* Description    : Request the CAN cell to enter Sleep mode
* Input 1        : CAN_WAKEUP_ON_EXT or CAN_WAKEUP_ON_CAN
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SleepRequest(int WakeupMode);

/*******************************************************************************
* Function Name  : CAN_SetUnusedMsgObj
* Description    : Configure the message object as unused
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SetUnusedMsgObj(int msgobj);

/*******************************************************************************
* Function Name  : CAN_SetTxMsgObj
* Description    : Configure the message object as TX
* Input 1        : message object number, from 0 to 31
* Input 2        : CAN_STD_ID or CAN_EXT_ID
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SetTxMsgObj(int msgobj, int idType);

/*******************************************************************************
* Function Name  : CAN_SetRxMsgObj
* Description    : Configure the message object as RX
* Input 1        : message object number, from 0 to 31
* Input 2        : CAN_STD_ID or CAN_EXT_ID
* Input 3        : low part of the identifier range used for acceptance filtering
* Input 4        : high part of the identifier range used for acceptance filtering
* Input 5        : TRUE for a single receive object or a FIFO receive object that
*                  is the last one of the FIFO
*                  FALSE for a FIFO receive object that is not the last one
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SetRxMsgObj(int msgobj, int idType, vu32 idLow, vu32 idHigh, bool singleOrFifoLast);

/*******************************************************************************
* Function Name  : CAN_InvalidateAllMsgObj
* Description    : Configure all the message objects as unused
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_InvalidateAllMsgObj(void);

/*******************************************************************************
* Function Name  : CAN_Init
* Description    : Initialize the CAN cell and set the bitrate
* Input 1        : any binary value formed from the CAN_CTL_xxx defines
* Input 2        : one of the CAN_BITRATE_xxx defines
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_Init(vu8 mask, int bitrate);

/*******************************************************************************
* Function Name  : CAN_ReleaseMessage
* Description    : Release the message object
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_ReleaseMessage(int msgobj);

/*******************************************************************************
* Function Name  : CAN_ReleaseTxMessage
* Description    : Release the transmit message object
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
* Note           : assume that message interface 0 is free
*******************************************************************************/
static inline void CAN_ReleaseTxMessage(int msgobj)
{
	CAN->sMsgObj[0].COMM = CAN_COM_CLRINT | CAN_COM_TXRQST;
	CAN->sMsgObj[0].COMR = 1 + msgobj;
}

/*******************************************************************************
* Function Name  : CAN_ReleaseRxMessage
* Description    : Release the receive message object
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
* Note           : assume that message interface 1 is free
*******************************************************************************/
static inline void CAN_ReleaseRxMessage(int msgobj)
{
	CAN->sMsgObj[1].COMM = CAN_COM_CLRINT | CAN_COM_TXRQST;
	CAN->sMsgObj[1].COMR = 1 + msgobj;
}

/*******************************************************************************
* Function Name  : CAN_SendMessage
* Description    : Start transmission of a message
* Input 1        : message object number, from 0 to 31
* Input 2        : pointer to the message structure containing data to transmit
* Output         : None
* Return         : 1 if transmission was OK, else 0
*******************************************************************************/
int  CAN_SendMessage(int msgobj, canmsg* pCanMsg);

/*******************************************************************************
* Function Name  : CAN_ReceiveMessage
* Description    : Get the message, if received
* Input 1        : message object number, from 0 to 31
* Input 2        : if TRUE, the message object is released when getting the data
*                  if FALSE, the message object is not released
* Input 3        : pointer to the message structure where received data is stored
* Output         : None
* Return         : 1 if reception was OK, else 0 (no message pending)
*******************************************************************************/
int  CAN_ReceiveMessage(int msgobj, bool release, canmsg* pCanMsg);

/*******************************************************************************
* Function Name  : CAN_WaitEndOfTx
* Description    : Wait until current transmission is finished
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_WaitEndOfTx(void);

/*******************************************************************************
* Function Name  : CAN_BasicSendMessage
* Description    : Start transmission of a message in BASIC mode
* Input 1        : pointer to the message structure containing data to transmit
* Output         : None
* Return         : 1 if transmission was OK, else 0
* Note           : CAN must be in BASIC mode
*******************************************************************************/
int CAN_BasicSendMessage(canmsg* pCanMsg);

/*******************************************************************************
* Function Name  : CAN_BasicReceiveMessage
* Description    : Get the message in BASIC mode, if received
* Input 1        : pointer to the message structure where received data is stored
* Output         : None
* Return         : 1 if reception was OK, else 0 (no message pending)
* Note           : CAN must be in BASIC mode
*******************************************************************************/
int CAN_BasicReceiveMessage(canmsg* pCanMsg);

/*******************************************************************************
* Function Name  : CAN_IsMessageWaiting
* Description    : Test the waiting status of a received message
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : A non-zero value if the corresponding message object has
*                  received a message waiting to be copied, else 0
*******************************************************************************/
static inline int CAN_IsMessageWaiting(int msgobj)
{
	return (msgobj < 16 ? CAN->NEWD1 & (1 << msgobj) : CAN->NEWD2 & (1 << (msgobj-16)));
}

/*******************************************************************************
* Function Name  : CAN_IsTransmitRequested
* Description    : Test the request status of a transmitted message
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : A non-zero value if the corresponding message is requested
*                  to transmit, else 0
*******************************************************************************/
static inline int CAN_IsTransmitRequested(int msgobj)
{
	return (msgobj < 16 ? CAN->TR1 & (1 << msgobj) : CAN->TR2 & (1 << (msgobj-16)));
}

/*******************************************************************************
* Function Name  : CAN_IsInterruptPending
* Description    : Test the interrupt status of a message object
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : A non-zero value if the corresponding message has an interrupt
*                  pending, else 0
*******************************************************************************/
static inline int CAN_IsInterruptPending(int msgobj)
{
	return (msgobj < 16 ? CAN->INTP1 & (1 << msgobj) : CAN->INTP2 & (1 << (msgobj-16)));
}

/*******************************************************************************
* Function Name  : CAN_IsObjectValid
* Description    : Test the validity of a message object (ready to use)
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : A non-zero value if the corresponding message object is valid,
*                  else 0
*******************************************************************************/
static inline int CAN_IsObjectValid(int msgobj)
{
	return (msgobj < 16 ? CAN->MSGV1 & (1 << msgobj) : CAN->MSGV2 & (1 << (msgobj-16)));
}

#endif /* __720_CAN_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
