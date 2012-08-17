///////////////////////////////////////////////////////////////////////////////	
//	Wrapper for ENC28J60 interrupt
//	akhe@eurosource.se, http://www.vscp.org
//
 
#include <p32xxxx.h>
#include <sys/asm.h>


#include "ISR_Support.h"
 
#define portEXC_CODE_MASK ( 0x1f << 2 )

	.set	nomips16
 	.set 	noreorder
 	
 	.extern vENC28K60ISR_Handler
 	
	.global vENC28K60ISR_Wrapper


/******************************************************************/

 	.section 	.FreeRTOS, "ax", @progbits
 	.set		noreorder
	.set 		noat
 	.ent		vENC28K60ISR_Wrapper
	
vENC28K60ISR_Wrapper:

	portSAVE_CONTEXT

	jal 		vENC28K60ISR_Handler
	nop

	portRESTORE_CONTEXT

	.end vENC28K60ISR_Wrapper



