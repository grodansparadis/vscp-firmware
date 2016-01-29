//********************************************************************
//                                                                    
// File name: VSCP_drv.c                                               
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   1.0                                 			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   General VSCP CAN Driver             
//                                                                    
//                                                                    
// History:                                                           
// 2007/03/07 [MAP] Create this file from PICos18 drv_can. Only change msg struct             
//                                                                   
//            PICos18 is free software; you can redistribute it       
//            and/or modify it under the terms of the GNU General     
//            Public License as published by the Free Software        
//            Foundation; either version 2, or (at your option)       
//            any later version.                                      
//                                                                    
//            PICos18 is distributed in the hope that it will be      
//            useful, but WITHOUT ANY WARRANTY; without even the      
//            implied warranty of MERCHANTABILITY or FITNESS FOR A    
//            PARTICULAR PURPOSE.  See the GNU General Public         
//            License for more details.                               
//                                                                    
//            You should have received a copy of the GNU General      
//            Public License along with gpsim; see the file           
//            COPYING.txt. If not, write to the Free Software         
//            Foundation, 59 Temple Place - Suite 330,                
//            Boston, MA 02111-1307, USA.                             
//                                                                    
//          > A special exception to the GPL can be applied should    
//            you wish to distribute a combined work that includes    
//            PICos18, without being obliged to provide the source    
//            code for any proprietary components.                    
//******************************************************************************

#include <Define_VSCP.h>
#include "VSCP_drv.h"

///////////////////////////////////////////////////////////////////////////////
//
//	OSCILLATOR	SPEED
//
#define	DEFAULT_CANSPEED		3
#define	OSCILATOR_FREQ			40

/*******************************************************************************
   CAN Baud rate setup table.
   Thanks Janez :-)
*******************************************************************************/

rom struct{    
   uint08_t  BRP;				//(1...64) Baud Rate Prescaler 
   uint08_t  SJW;				//(1...4) SJW time
   uint08_t  PROP;			//(1...8) PROP time
   uint08_t  PhSeg1;		//(1...8) Phase Segment 1 time
   uint08_t  PhSeg2;		//(1...8) Phase Segment 2 time
//   BRP,SJW,PROP,PH1,PH1
}

CO_BitRateData[8] = 
#if OSCILATOR_FREQ == 16
//   BRP,SJW,PROP,PH1,PH1
   {50,  1, 4, 8, 3, 			//CAN=10kbps
    25,  1, 4, 8, 3, 			//CAN=20kbps
    10,  1, 4, 8, 3, 			//CAN=50kbps
    4,   1, 4, 8, 3, 			//CAN=125kbps
    2,   1, 4, 8, 3, 			//CAN=250kbps
    1,   1, 4, 8, 3, 			//CAN=500kbps
    1,   1, 2, 4, 3, 			//CAN=800kbps
    1,   1, 2, 3, 2};			//CAN=1000kbps
#elif OSCILATOR_FREQ == 20
//   BRP,SJW,PROP,PH1,PH1
   {50,  1, 6, 8, 5, 			//CAN=10kbps
    25,  1, 6, 8, 5, 			//CAN=20kbps
    10,  1, 6, 8, 5, 			//CAN=50kbps
    5,   1, 4, 8, 3, 			//CAN=125kbps
    2,   1, 6, 8, 5, 			//CAN=250kbps
    1,   1, 6, 8, 5, 			//CAN=500kbps
    1,   1, 3, 5, 3, 			//CAN=800kbps     //combination does not work
    1,   1, 2, 4, 3};			//CAN=1000kbps
#elif OSCILATOR_FREQ == 32
//   BRP,SJW,PROP,PH1,PH1
   {64,  1, 8, 8, 8, 			//CAN=10kbps
    50,  1, 4, 8, 3, 			//CAN=20kbps
    20,  1, 4, 8, 3, 			//CAN=50kbps
    8,   1, 4, 8, 3, 			//CAN=125kbps
    4,   1, 4, 8, 3, 			//CAN=250kbps
    2,   1, 4, 8, 3, 			//CAN=500kbps
    2,   1, 2, 4, 3, 			//CAN=800kbps
    2,   1, 2, 3, 2};			//CAN=1000kbps
#elif OSCILATOR_FREQ == 40
//   BRP,	SJW,	PROP,	PH1,	PH1
   {64,  1, 8, 8, 8, 			//CAN=12,5Kbps//	OK MAP
    50,  1, 6, 8, 5, 			//CAN=20kbps	//	OK MAP
    25,  1, 4, 8, 3, 			//CAN=50kbps	//	OK MAP
    8,   2, 4, 8, 6, 			//CAN=125kbps	//	Modify MAP OK 
    4,   2, 4, 8, 6, 			//CAN=250kbps	//	Modify MAP OK
    2,   2, 4, 8, 6, 			//CAN=500kbps	//	Modify MAP OK
    0,   0, 0, 0, 0, 			//CAN=800kbps
    0,   0, 0, 0, 0};			//CAN=1000kbps
#else
   #error define_OSCILATOR_FREQ OSCILATOR_FREQ not supported
#endif

/**********************************************************************
 * Definition dedicated to the local functions.
 **********************************************************************/
extern uint08_t vscp_nickname;	// Node nickname
EventMaskType   	VSCP_event;
uint08_t	 Receiver_Manager;

uint08_t	 RXQueueInput;		//	increment with en-queue
uint08_t 	 RXQueueOutput;		//	increment with de-queue
uint08_t   RXQueueCount;		//	Number of items currently in queue
VSCP_msg_t RXQueue[VSCP_RX_QUEUE_SIZE];

uint08_t	 TXQueueInput;		//	increment with en-queue
uint08_t	 TXQueueOutput;		//	increment with de-queue
uint08_t   TXQueueCount;		//	Number of items currently in queue
VSCP_msg_t TXQueue[VSCP_TX_QUEUE_SIZE];

uint08_t	BaudRate;

/**********************************************************************
 * Task of the CAN driver, waiting for any of these 4 events :
 *  VSCP_NEW_MSG   : A message has been sent then a hardware transmitter
 *                  is free. The driver will tranfer a software buffer
 *                  into a free TX[0,1,2] hardware buffer.
 *  VSCP_RCV_MSG   : A message is arrived in the RX[0,1] buffer.
 *                  the driver transfer the content of the message into
 *                  a software buffer waiting for this message ID or
 *                  discard the message to free the hardware.
 *  VSCP_ERR_MSG   : Something wrong appended and the driver must clean
 *                  the hardware before being disconnected. To be
 *                  completed...
 *
 * @param  void     
 * @return void   
 **********************************************************************/
TASK(VSCP_DrvCAN)
{
  // Determine CAN bus Speed
  VSCP_FindSpeed(&BaudRate);
  VSCP_config();

  while(1)
  {
    WaitEvent(VSCP_NEW_MSG | VSCP_RCV_MSG | VSCP_ERR_MSG);
    GetEvent(VSCP_DRV_ID, &VSCP_event);        

    if (VSCP_event & VSCP_NEW_MSG)
    {
      ClearEvent(VSCP_NEW_MSG);
      WriteCANBuffer();      						//pending: tx error process
    }
    
    if (VSCP_event & VSCP_RCV_MSG)
    {
      ClearEvent(VSCP_RCV_MSG);
      ReadCANBuffer();   
    }
    
    if (VSCP_event & VSCP_ERR_MSG)
    {
      ClearEvent(VSCP_ERR_MSG);
		}
  }
}
 
/**********************************************************************
 * Hardware settings of the CAN port.
 *
 * @param  void     
 * @return void   
 **********************************************************************/
void VSCP_config(void)
{
  CANCON   = 0x80;
  TRISBbits.TRISB2 = 0;
  TRISBbits.TRISB3 = 1;
  
  VSCP_SetSpeed(BaudRate);

  CIOCON   = 0x20;
  PIE3     = 0xE3;/* All RX and Err without TX enabled */
  IPR3     = 0x00;
  RXM0SIDL = 0;
  RXM0SIDH = 0;
  RXM0EIDL = 0x00;
  RXM0EIDH = 0x00;
  RXF0SIDL = 0x00;
  RXF0SIDH = 0x00;
  RXF0EIDL = 0x00;
  RXF0EIDH = 0x00;
  RXB0CON  = 0x64; /* 0x24 for STD ID, 0x64 for EXT ID */
  TXB0CON  = 0x00;
  CANCON   = 0x00;

  /* Settings for the first frame to send */
  PIE3bits.TXB0IE = 0;

  /* Init the different FIFO */
	RXQueueInput = 0;					
 	RXQueueOutput = 0;					
  RXQueueCount = 0;				

	TXQueueInput = 0;					
 	TXQueueOutput = 0;					
  TXQueueCount = 0;				
  Receiver_Manager = 0;
}


/**********************************************************************
 * Called by the CAN driver task.
 * Function is incomplete now, finish later.
 * For now, just return OK and continue...
 *
 * @param  void        
 * @return Status    Always return E_OK
 **********************************************************************/
StatusType VSCP_FindSpeed(uint08_t * speed)
{
	*speed = DEFAULT_CANSPEED;		//	Set to default
	return E_OK;
}


/**********************************************************************
 * Sets the various baud rate registers based upon the CO_BitRateData table..
 *
 * @param  void        
 * @return Status    Always return E_OK
 **********************************************************************/
StatusType VSCP_SetSpeed(uint08_t speed)
{
	
	if ((speed > 7) || (speed < 0))
		return E_OS_STATE ;
		
	//	Prescaler in lower 6 bits; bits 5:0.  BRP as coded, begins at 1, so
	//	you must subtract 1 to convert	
	BRGCON1 = CO_BitRateData[speed].BRP -1;	
	
	//	Set the syncronizaiton jump width
	BRGCON1 += (CO_BitRateData[speed].SJW -1)<< 6;
	
	//	Propigation in lower 3 bits, 2:0
	BRGCON2 = CO_BitRateData[speed].PROP - 1;
	
	//	Located bits 5:3		
	BRGCON2 += (CO_BitRateData[speed].PhSeg1 -1) << 3;
	
	BRGCON2bits.SEG2PHTS = 1;	
	
	//	Set phase 2 segment time
	BRGCON3 = CO_BitRateData[speed].PhSeg2 -1;
	
	return E_OK;
}


/**********************************************************************
 * Called by the CAN driver task.
 * Call the CopyFrmaeBuffer2Hard 3 times, one for each TX buffer.
 * Indeed the 3 buffers could be free then we have to take our chance 
 * to send 3 waiting frames at the same time.
 *
 * @param  void        
 * @return Status    Always return E_OK
 **********************************************************************/
StatusType WriteCANBuffer(void)
{
	pTXBUF_t	txBuf;

	while (TXQueueCount)
	{
		// Find an empty buffer
			txBuf = TXBUF_START;
			while (((txBuf->TXCON & TXCON_EMPTY) != 0) &&	(txBuf >=TXBUF_END)){
				txBuf--;
			}

	//	If we reached the end before finding an empty, return immed.	
		if (txBuf < TXBUF_END)
			return E_OS_STATE;
	
		CopyFrameBuffer2Hard(txBuf);
  }
  return(E_OK);
}

/**********************************************************************
 *	Set the receiver task ID
 *
 *	Once the Rx message is enqueue in the Rx queue, 
 *  an event is sent to the Receiver_Manager (the task that call this Function)
 *
 **********************************************************************/
StatusType VSCP_RCV_Register( void)
{
    Receiver_Manager = id_tsk_run;
    /* Restart the different FIFO for clear operation*/
		RXQueueInput = 0;					
	 	RXQueueOutput = 0;					
	  RXQueueCount = 0;				
	
		TXQueueInput = 0;					
	 	TXQueueOutput = 0;					
	  TXQueueCount = 0;				

    return E_OK;
}


/**********************************************************************
 * Once a such message is found we post an event to the sending task
 * to tell the transfer is started and we break.
 * The event is used if the task want to send as fast as possible a 
 * set of message. Doing so the task will be able to send a new message
 * every time a TASK_VSCP_TX_EVENT event is received.
 *
 * @param  type    IN Number of the hardware buffer      
 * @return Status     E_OK if tranfer initiated
 *                    E_OS_STATE otherwise
 **********************************************************************/
StatusType CopyFrameBuffer2Hard(pTXBUF_t pTxbuf)
{
  StatusType returned_type;
	VSCP_msg_t VSCP_current_message;

  returned_type = E_OS_STATE;

  if (VSCP_deqMsgTx( &VSCP_current_message)== E_OS_STATE)
    return(returned_type);
  
  pTxbuf->EIDL 		= VSCP_current_message.nickname;
  pTxbuf->EIDH 		= VSCP_current_message.vscp_type;
  pTxbuf->SIDL 		= (VSCP_current_message.vscp_class & 0x03)|((VSCP_current_message.vscp_class << 3)& 0xE0)| 0x08;  //set Extended Identifier Enable Bit
  pTxbuf->SIDH 		= ((VSCP_current_message.vscp_class >> 5) & 0x0f)|((VSCP_current_message.priority << 5) & 0xE0);
  pTxbuf->DLC  		= VSCP_current_message.length;
  pTxbuf->DATA[0] = VSCP_current_message.data[0];
  pTxbuf->DATA[1] = VSCP_current_message.data[1];
  pTxbuf->DATA[2] = VSCP_current_message.data[2];
  pTxbuf->DATA[3] = VSCP_current_message.data[3];
  pTxbuf->DATA[4] = VSCP_current_message.data[4];
  pTxbuf->DATA[5] = VSCP_current_message.data[5];
  pTxbuf->DATA[6] = VSCP_current_message.data[6];
  pTxbuf->DATA[7] = VSCP_current_message.data[7];
	
  SET_TXREQ(pTxbuf);

  returned_type = E_OK;
  
  return(returned_type);
}

/**********************************************************************
 *	Enqueue a client packet object into the TX queue.
 *
 *	Once placed in queue, client must not modify the data
 *	otherwise unpredictable results. To safely change the object,
 *	dequeue, modify, re-enqueue.
 *
 *  The code in mainly executed in critical region [SuspendAllInterrupt]
 *  because many tasks can call this function at the same time and break
 *  the FIFO list.
 *
 *	Returns 1 if successfull, 0 if message could not be enqueued
 **********************************************************************/
StatusType VSCP_enqMsgTx(VSCP_msg_tRef toEnqueue, uint08_t RxEnqueue)
{

  if (TXQueueCount < VSCP_TX_QUEUE_SIZE )
  {
    SuspendOSInterrupts();
		TXQueueCount++;
		toEnqueue->nickname = vscp_nickname;
		
		TXQueue[TXQueueInput++] = *toEnqueue;

		if (TXQueueInput >= VSCP_TX_QUEUE_SIZE)
			TXQueueInput = 0;
	
    ResumeOSInterrupts();
    if (RxEnqueue)
    {
		  VSCP_enqMsgRx( toEnqueue);
		  SetRxEvent();    
  	}
    SetEvent(VSCP_DRV_ID, VSCP_NEW_MSG);
    return E_OK;
  }
  else
    return E_OS_STATE;
}

/**********************************************************************
 *	Dequeue a client message from the VSCP Tx task queue.
 *
 *
 *********************************************************************/
StatusType VSCP_deqMsgTx(VSCP_msg_tRef toDequeue)
{
  if ( TXQueueCount > 0 )
  {
    SuspendOSInterrupts();
		*toDequeue = TXQueue[TXQueueOutput++];
		if (TXQueueOutput >= VSCP_TX_QUEUE_SIZE)
			TXQueueOutput = 0;
		TXQueueCount--;
    ResumeOSInterrupts();
    return E_OK;
  }
  else
    return E_OS_STATE;
} 

/**********************************************************************
 *	Enqueue a client packet object into the RX queue.
 *
 *	Once placed in queue, client must not modify the data
 *	otherwise unpredictable results. To safely change the object,
 *	dequeue, modify, re-enqueue.
 *
 *  The code in mainly executed in critical region [SuspendAllInterrupt]
 *  because many tasks can call this function at the same time and break
 *  the FIFO list.
 *
 *	Returns 1 if successfull, 0 if message could not be enqueued
 **********************************************************************/
StatusType VSCP_enqMsgRx(VSCP_msg_tRef toEnqueue)
{
  if (RXQueueCount < VSCP_RX_QUEUE_SIZE)
  {
    SuspendOSInterrupts();
		RXQueueCount++;
		RXQueue[RXQueueInput++] = *toEnqueue;
		if (RXQueueInput >= VSCP_RX_QUEUE_SIZE)
			RXQueueInput = 0;
    ResumeOSInterrupts();
    return E_OK;
  }
  else
    return E_OS_STATE;
}

/**********************************************************************
 *	Dequeue a client message from the VSCP Rx task queue.
 *
 *
 *********************************************************************/
StatusType VSCP_deqMsgRx(VSCP_msg_tRef toDequeue)
{
  if ( RXQueueCount > 0 )
  {
    SuspendOSInterrupts();
		*toDequeue = RXQueue[RXQueueOutput++];
		if (RXQueueOutput >= VSCP_RX_QUEUE_SIZE)
			RXQueueOutput = 0;
		RXQueueCount--;
    ResumeOSInterrupts();
    return E_OK;
  }
  else
    return E_OS_STATE;
} 
/**********************************************************************
 *	Return RXQueueCount, count RX message in the VSCP Rx queue.
 *
 *
 *********************************************************************/
uint08_t VSCP_CountMsgRx()
{
    return RXQueueCount;
} 

/**********************************************************************
 * Called by the CAN driver task.
 * Check the RXFUL bit of the both RX CAN hardware buffers.
 * If at least one message is store in hardware then this function
 * tries to tranfer the data from hardware to dedicated structure
 * (structure created by other tasks).
 *
 * @param void        
 * @return Status    Always return E_OK
 **********************************************************************/
StatusType ReadCANBuffer(void)
{
  if (PIR3bits.RXB0IF == 1)
  {
    CANCONbits.WIN2 = 1; 
    CANCONbits.WIN1 = 1; 
    CANCONbits.WIN0 = 0;
    if (CopyHard2FrameBuffer() == E_OK)
    {
      RXB0CONbits.RXFUL = 0;
      PIR3bits.RXB0IF    = 0;
      PIE3bits.RXB0IE   = 1;
    }
  }
  if (PIR3bits.RXB1IF == 1)
  {
    CANCONbits.WIN2 = 1; 
    CANCONbits.WIN1 = 0; 
    CANCONbits.WIN0 = 1;
    if (CopyHard2FrameBuffer() == E_OK)
    {
      RXB0CONbits.RXFUL = 0;
      PIR3bits.RXB1IF    = 0;
      PIE3bits.RXB1IE   = 1;
    }
  }
  
  return(E_OK);
}


/**********************************************************************
 * Parse the entier RX message list to find any task waiting for the 
 * received message ID.
 * Once a such taks is found we post an event to the waiting task
 * to tell its software buffer is full and we break.
 * The event is used if the task want to wait for a particular message
 * ID without overloading the CPU by a pooling method.
 *
 * @return Status     E_OK ever. If Queue isfullMSG is missing
 **********************************************************************/
StatusType CopyHard2FrameBuffer(void)
{
	VSCP_msg_t VSCP_current_message;
   
  VSCP_current_message.priority = (RXB0SIDH & 0xE0)>>5;
  VSCP_current_message.vscp_class = (uint16_t)((RXB0SIDL & 0xE0)>>3) + (uint16_t)(RXB0SIDL & 0x03);
  VSCP_current_message.vscp_class += (uint16_t)(((uint16_t)RXB0SIDH & 0x000F)<<5);
  VSCP_current_message.vscp_type = RXB0EIDH;
  VSCP_current_message.nickname = RXB0EIDL;
  VSCP_current_message.length  = RXB0DLC & 0x0F;
  VSCP_current_message.data[0] = RXB0D0;
  VSCP_current_message.data[1] = RXB0D1;
  VSCP_current_message.data[2] = RXB0D2;
  VSCP_current_message.data[3] = RXB0D3;
  VSCP_current_message.data[4] = RXB0D4;
  VSCP_current_message.data[5] = RXB0D5;
  VSCP_current_message.data[6] = RXB0D6;
  VSCP_current_message.data[7] = RXB0D7;

  VSCP_enqMsgRx( &VSCP_current_message);
  SetRxEvent();
	return(E_OK);
}

/**********************************************************************
 *
 * @return Status     E_OK ever. If Queue isfullMSG is missing
 **********************************************************************/
void SetRxEvent(void)
{

  if ( Receiver_Manager)
 		SetEvent(Receiver_Manager, VSCP_QUEUE_RX);
}


/**********************************************************************
 * ISR of the CAN driver.
 * 
 * For each interrupt we disable the IE first to avoid any infinite loop
 * and we clear the IF in the dedicated function (Read or Write CAN mess).
 * Indeed we have to clear the IT when the message has been properly 
 * accessed and is no more used. The IF lets the peripheral accept a new 
 * message.  
 * If 3 frames arrive at the same time, the two first are stored in the 
 * RCV buffers and the third is discarded (temporary overload bus).
 *
 * @return void 
 **********************************************************************/
void VSCP_INT(void)
{
  if (PIR3 & 0x03)
  { 
    if (PIR3bits.RXB0IF)
      PIE3bits.RXB0IE = 0;
    if (PIR3bits.RXB1IF)
      PIE3bits.RXB1IE = 0;
    SetEvent(VSCP_DRV_ID, VSCP_RCV_MSG);
  };
  
  if (PIR3 & 0x1C)
  {
    if (PIR3bits.TXB0IF)
      PIE3bits.TXB0IE = 0;
    if (PIR3bits.TXB1IF)
      PIE3bits.TXB1IE = 0;
    if (PIR3bits.TXB2IF)
      PIE3bits.TXB2IE = 0;
    SetEvent(VSCP_DRV_ID, VSCP_NEW_MSG);
  }
    
  if (PIR3 & 0xE0)
  {
    if (PIR3bits.WAKIF)
      PIR3bits.WAKIF = 0;
    if (PIR3bits.ERRIF)
    {
      COMSTAT = 0;
      PIR3bits.ERRIF = 0;
    }
    if (PIR3bits.IRXIF)
      PIR3bits.IRXIF = 0;
    SetEvent(VSCP_DRV_ID, VSCP_ERR_MSG);
  }
}

/* End of File : drv_can.c */

