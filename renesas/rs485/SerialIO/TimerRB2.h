/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2012 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
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
* TimerRB2.c - R8CM12A TimerRB2 Function Interface
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  2-16-12	First written.
*******************************************************************************                
*/                                                                                             

#ifndef	_TIMERRB2_H_
#define	_TIMERRB2_H_

/* TIMER RB2 Interrupt Processes */
void TimerRB2_Process(void);

/* initialize Timer RB2 */
void TimerRB2_Init(void);

//void TimerRB2_OneShotWait(void);
void TimerRB2_Start(void);
BOOLEAN TimerRB2_IsExpired(void);
void TimerRB2_Stop(void);

#endif /* _TIMERRB2_H_ */
