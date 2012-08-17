//
//  $Id: swi.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/swi/swi.h $
//

#ifndef _SWI_H_
#define _SWI_H_

//
//
//
typedef enum
{
  SWICALL_YIELDPROCESSOR = 0,   // FreeRTOS xYieldTask (asm)
  SWICALL_A_LED2SET,            // Set LED2 on or off, return previous state (asm)
  SWICALL_A_LED2ON,             // LED2 on, return previous state (asm)
  SWICALL_A_LED2OFF,            // LED2 off, return previous state (asm)
  SWICALL_A_LED2TOGGLE,         // LED2 toggle, return previous state (asm)
  SWICALL_C_LED2SET,            // Set LED2 on or off, return previous state (C)
  SWICALL_C_LED2ON,             // LED2 on, return previous state (C)
  SWICALL_C_LED2OFF,            // LED2 off, return previous state (C)
  SWICALL_C_LED2TOGGLE,         // LED2 toggle, return previous state (C)
}
swiCalls_e;

//
//
//
#define SWICALL(swiID, swiOut) \
	asm  volatile (\
	"swi %a1   \n\t" \
	"mov %0,r0 \n\t" \
	: "=r" (swiOut) : "I" (swiID) : "r0", "lr")

#define SWICALL1(swiID, swiIn, swiOut) \
	asm  volatile (\
	"mov r0,%1 \t\n" \
	"swi %a2   \n\t" \
	"mov %0,r0 \n\t" \
	: "=r" (swiOut) : "r" (swiIn), "I" (swiID) : "r0", "lr")

#define SWICALL2(swiID, swiIn1, swiIn2, swiOut) \
	asm volatile ( \
	"mov r0,%1 \t\n" \
	"mov r1,%2 \t\n" \
	"swi %a3   \n\t" \
	"mov %0,r0 \n\t" \
	: "=r" (swiOut) : "r" (swiIn1), "r" (swiIn2), "I" (swiID) : "r0", "lr")

#define SWICALL3(swiID, swiIn1, swiIn2, swiIn3, swiOut) \
	asm volatile ( \
	"mov r0,%1 \t\n" \
	"mov r1,%2 \t\n" \
	"mov r2,%3 \t\n" \
	"swi %a4   \n\t" \
	"mov %0,r0 \n\t" \
	: "=r" (swiOut) : "r" (swiIn1), "r" (swiIn2), "r" (swiIn3), "I" (swiID) : "r0", "lr")

//
//
//
void swiInit (void);
int swiDispatchC (unsigned long r0, unsigned long r1, unsigned long r2, unsigned long swi);

#endif
