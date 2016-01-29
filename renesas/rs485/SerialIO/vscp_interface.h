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
* vscp_interface.c - ACS Serial I/O VSCP Interface Software Implementation
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  6-24-13	First written.
*******************************************************************************                
*/

#ifndef	_VSCP_INTERFACE_H_
#define	_VSCP_INTERFACE_H_

#define	APP_REG_FIRST_CONTACT			0
#define	APP_REG_LAST_CONTACT			(MAX_CONTACTS - 1)
#define	APP_REG_ZONE					57
#define	APP_REG_SUBZONE					58
#define	APP_REG_DECISION_MATRIX			64

#define	MAX_DECISION_ROWS	8

#define	APP_REG_PAGE_CONTACTS			0
#define	APP_REG_PAGE_CLOSURES			1
#define	APP_REG_PAGE_OPENINGS			2
#define	APP_REG_PAGE_REPORT_ON			3
#define	APP_REG_PAGE_REPORT_OFF			4
#define	APP_REG_PAGE_REPORT_CLOSURES	5
#define	APP_REG_PAGE_REPORT_OPENINGS	6
#define	APP_REG_PAGE_REPORT_SUBZONES	7
#define	MAX_APP_REG_PAGES				8	// must match # of APP_REG_PAGE_xxx entries

#define	ACTION_NOP		0
#define	ACTION_OFF		1
#define	ACTION_ON		2
#define	ACTION_TOGGLE	3
#define	ACTION_STATUS	4

void vscp_doDecisionMatrix(void);
void vscp_sendInformationEvent(unsigned char index, unsigned char eventClass, unsigned char eventType);

#endif	/* _VSCP_INTERFACE_H_ */


