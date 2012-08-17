/*============================== */
/*  ADuC7026 HEADER FILE REV 0.3 */
/*============================== */

#ifndef ADUC7026		/*	Define ADuC7026 */
#define ADUC7026 
#endif

#include <irq.h>		/* 	Interrupt Controller 		*/

/*  REMAP AND SYSTEM CONTROL  */

#define REMAPBASE 	(*(volatile unsigned long *) 	0XFFFF0200)
#define REMAP 		(*(volatile unsigned long *) 	0XFFFF0220)
#define RSTSTA 		(*(volatile unsigned long *) 	0XFFFF0230)
#define RSTCLR 		(*(volatile unsigned long *) 	0XFFFF0234)

#include <timer.h>		/*  	TIMER 0    			*/

/*  PLL AND OSCILLATOR CONTROL  */

#define PLLBASE 	(*(volatile unsigned long *) 	0XFFFF0400)
#define POWKEY1		(*(volatile unsigned long *) 	0XFFFF0404)
#define POWCON 		(*(volatile unsigned long *) 	0XFFFF0408)
#define POWKEY2		(*(volatile unsigned long *) 	0XFFFF040C)
#define PLLKEY1		(*(volatile unsigned long *) 	0XFFFF0410)
#define PLLCON 		(*(volatile unsigned long *) 	0XFFFF0414)
#define PLLKEY2		(*(volatile unsigned long *) 	0XFFFF0418)

/*  POWER SUPPLY MONITOR   */

#define PSMBASE 	(*(volatile unsigned long *) 	0XFFFF0440)
#define PSMCON 		(*(volatile unsigned long *) 	0XFFFF0440)
#define CMPCON 		(*(volatile unsigned long *) 	0XFFFF0444)

/*	Band Gap Reference */

#define REFBASE 	(*(volatile unsigned long *) 	0XFFFF0480)
#define REFCON 		(*(volatile unsigned long *) 	0XFFFF048C)

/*  ADC INTERFACE REGISTERS   */

#define ADCBASE 	(*(volatile unsigned long *)	0XFFFF0500)
#define ADCCON 		(*(volatile unsigned long *) 	0XFFFF0500)
#define ADCCP 		(*(volatile unsigned long *) 	0XFFFF0504)
#define ADCCN 		(*(volatile unsigned long *) 	0XFFFF0508)
#define ADCSTA 		(*(volatile unsigned long *) 	0XFFFF050C)
#define ADCDAT 		(*(volatile unsigned long *) 	0XFFFF0510)
#define ADCRST 		(*(volatile unsigned long *) 	0XFFFF0514)
#define ADCGN 		(*(volatile unsigned long *) 	0XFFFF0530)
#define ADCOF 		(*(volatile unsigned long *) 	0XFFFF0534)

/* EXTERNAL MEMORY REGISTERS */

#define XMBASE		(*(volatile unsigned long *)	0XFFFFF000)
#define XMCFG		(*(volatile unsigned long *)	0XFFFFF000)
#define XM0CON		(*(volatile unsigned long *)	0XFFFFF010)
#define XM1CON		(*(volatile unsigned long *)	0XFFFFF014)
#define XM2CON		(*(volatile unsigned long *)	0XFFFFF018)
#define XM3CON		(*(volatile unsigned long *)	0XFFFFF01C)
#define XM0PAR		(*(volatile unsigned long *)	0XFFFFF020)
#define XM1PAR		(*(volatile unsigned long *)	0XFFFFF024)
#define XM2PAR		(*(volatile unsigned long *)	0XFFFFF028)
#define XM3PAR		(*(volatile unsigned long *)	0XFFFFF02C)

#include <dac.h>		/* 	DAC Registers 				*/

#include <uart.h>		/*  	450 COMPATIABLE UART CORE REGISTERS 	*/

#include <i2c.h>		/*  	I2C BUS PERIPHERAL DEVICE  		*/

/*  SERIAL PORT INTERFACE PERIPHERAL  */

#define SPIBASE 	(*(volatile unsigned long *) 	0XFFFF0A00)
#define SPISTA  	(*(volatile unsigned long *) 	0XFFFF0A00)
#define SPIRX 		(*(volatile unsigned long *) 	0XFFFF0A04)
#define SPITX 		(*(volatile unsigned long *) 	0XFFFF0A08)
#define SPIDIV 		(*(volatile unsigned long *) 	0XFFFF0A0C)
#define SPICON 		(*(volatile unsigned long *) 	0XFFFF0A10)

#include <pla.H>		/*  	PROGRAMABLE LOGIC ARRAY   		*/

#include <gpio.h>		/*  	GPIO AND SERIAL PORT MUX 		*/

#include <flash.h>		/* 	Flash Control Interface 		*/

#include <pwm.h>		/*	PWM					*/

#ifndef __KEIL__

void  	(*IRQ)(void);		/* 	Function Pointers for Interupts 	*/
void 	(*SWI)(void);
void 	(*FIQ)(void);
void  	(*UNDEF)(void);
void 	(*PABORT)(void);
void 	(*DABORT)(void);

#endif


