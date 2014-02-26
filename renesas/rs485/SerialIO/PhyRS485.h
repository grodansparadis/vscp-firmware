/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2013-2014 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
*                                                                                              
*       This Information is Proprietary to Ackerman Computer Sciences, Sarasota,               
* Florida (ACS) and MAY NOT be copied by any method or incorporated into another               
* program without the express written consent of ACS. This Information or any                  
* portion thereof remains the property of ACS. The Information contained herein                
* is believed to be accurate and ACS assumes no responsibility or liability for                
* its use in any way and conveys no license or title under any patent or                       
* copyright and makes no representation or warranty that this Information is                   
* free from patent or copyright infringement.                                                  
*------------------------------------------------------------------------------                
* PhyRS485.h - ACS Serial I/O RS-485 Physical Layer Interface
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*   6-07-13	First written.
*******************************************************************************                
*/                                                                                             

BOOLEAN PHY_RS485_IsReceivedFrameAvailable(BYTE *nodeId);
BYTE PHY_RS485_ReceiveFrameData(void);
BOOLEAN PHY_RS485_SendRoomForFrame(BYTE count);
void PHY_RS485_InitSendFrame(BYTE destinationAddress);
void PHY_RS485_SendFrameData(BYTE data);
void PHY_RS485_FinishSendFrame(void);
BOOLEAN PHY_RS485_IsSending(void);
void PHY_RS485_Init(void);
void PHY_RS485_Process(void);

