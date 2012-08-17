///////////////////////////////////////////////////////////////////////////////	
//	Wrapper for MRF24J40 interrupt
//	akhe@eurosource.se, http://www.vscp.org
//
 
#include <p32xxxx.h>
#include <sys/asm.h>


#include "ISR_Support.h"
 
#define portEXC_CODE_MASK ( 0x1f << 2 )

	.set	nomips16
 	.set 	noreorder
 	
 	.extern vMRF24J40ISR_Handler
 	
	.global vMRF24J40ISR_Wrapper


/******************************************************************/

 	.section 	.FreeRTOS, "ax", @progbits
 	.set		noreorder
	.set 		noat
 	.ent		vMRF24J40ISR_Wrapper
	
vMRF24J40ISR_Wrapper:

	portSAVE_CONTEXT

	jal 		vMRF24J40ISR_Handler
	nop

	portRESTORE_CONTEXT

	.end vMRF24J40ISR_Wrapper



