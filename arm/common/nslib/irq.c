/* 
 * irq.c --
 *
 *	Routines to manage and dispatch IRQ interrupt service routines.
 *	This file is partly based on code by Marco Graziano.
 *
 * Copyright (c) 1996 Ben Williamson.
 * All rights reserved.
 *
 * This file is part of nslib, a library used by programs
 * running on the Netslate Development Board.
 *
 * This software is released under the GNU Public License.
 * See the file COPYING for more information.
 */

#include <nslib.h>

static PFV	irqVector[IRQ_MAX_NUM];



/*
 *----------------------------------------------------------------------
 *
 * IrqInit --
 *
 *	Initialise the ARM7500 interrupt masks and IRQ vector table.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	All interrupts are masked off.  The vector table is filled with
 *	NULL pointers.
 *
 *----------------------------------------------------------------------
 */

void
IrqInit(void)
{
	int i;

	IntDisable();

	/*
	 * Mask off all interrupts
	 */
	IRQMSKA = 0;
	IRQMSKB = 0;
	IRQMSKC = 0;
	IRQMSKD = 0;
	DMAMSK  = 0;
	FIQMSK  = 0;

	/*
	 * Clear the interrupt vector table
	 */
	for (i = 0; i < IRQ_MAX_NUM; i++) {
		irqVector[i] = NULL;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * IrqEnable --
 *
 *	Enable a particular interrupt source in the IRQ mask registers.
 *	Expand this to a switch statement if you want speed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The mask bit in the IRQMSK* register appropriate to interrupt
 *	source number irqNum is set.
 *
 *----------------------------------------------------------------------
 */

void
IrqEnable(int irqNum)
{
	if ((7 >= irqNum) && (irqNum >= 0))
		IRQMSKA = IRQMSKA | (1 << irqNum);
	else if ((15 >= irqNum) && (irqNum >= 8))
		IRQMSKB = IRQMSKB | (1 << (irqNum - 8));
	else if ((23 >= irqNum) && (irqNum >= 16))
		IRQMSKC = IRQMSKC | (1 << (irqNum - 16));
	else if ((31 >= irqNum) && (irqNum >= 24))
		IRQMSKD = IRQMSKD | (1 << (irqNum - 24));
	else if (irqNum == 36)
		DMAMSK = DMAMSK | (1 << (36 - 32));
}

/*
 *----------------------------------------------------------------------
 *
 * IrqDisable --
 *
 *	Disable a particular interrupt source in the IRQ mask registers.
 *	Expand this to a switch statement if you want speed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The mask bit in the IRQMSK* register appropriate to interrupt
 *	source number irqNum is cleared.
 *
 *----------------------------------------------------------------------
 */

void              
IrqDisable(int irqNum)
{
	if ((7 >= irqNum) && (irqNum >= 0))
		IRQMSKA = IRQMSKA & ~(1 << irqNum);
	else if ((15 >= irqNum) && (irqNum >= 8))
		IRQMSKB = IRQMSKB & ~(1 << (irqNum - 8));
	else if ((23 >= irqNum) && (irqNum >= 16))
		IRQMSKC = IRQMSKC & ~(1 << (irqNum - 16));
	else if ((31 >= irqNum) && (irqNum >= 24))
		IRQMSKD = IRQMSKD & ~(1 << (irqNum - 24));
	else if (irqNum == 36)
		DMAMSK = DMAMSK & ~(1 << (36 - 32));
}

/*
 *----------------------------------------------------------------------
 *
 * IrqInstall --
 *
 *	Install and enable a new interrupt service routine.
 *
 * Results:
 *	Returns a pointer to the old installed handler, or NULL if none.
 *
 * Side effects:
 *	From now on, interrupts from the specified interrupt
 *	source will cause the new handler to be called.
 *
 *----------------------------------------------------------------------
 */

PFV
IrqInstall(int irqNum, PFV isrProc)
{
	PFV oldIsr;
 
	if (irqNum < 0 || IRQ_MAX_NUM <= irqNum) {
		return(NULL);
	}
	oldIsr = irqVector[irqNum];
	irqVector[irqNum] = isrProc;
	IrqEnable(irqNum);
	return(oldIsr);
}

/*
 *----------------------------------------------------------------------
 *
 * IrqDispatch --
 *
 *	Determine the cause of an IRQ interrupt and call the appropriate
 *	high level handler.  Called by the assembly TrapIrq routine.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Whatever the high level handler does.  The interrupt serviced
 *	is the highest priority one found in the IRQ request registers.
 *
 *----------------------------------------------------------------------
 */

void IrqDispatch(void)
{
	uint 	rq;
	PFV	isr;

	rq = IRQRQA & 0xFF;
	if (rq != 0) {
		isr = irqVector[unmapTbl[rq]];
		if (isr) {
			(*(isr))();
		}
		return;
	}

	rq = IRQRQB & 0xFF;
	if (rq != 0) {
		isr = irqVector[unmapTbl[rq] + 8];
		if (isr) {
			(*(isr))();
		}
		return;
	}

	rq = IRQRQC & 0xFF;
	if (rq != 0) {
		isr = irqVector[unmapTbl[rq] + 16];
		if (isr) {
			(*(isr))();
		}
		return;
	}

	rq = IRQRQD & 0xFF;
	if (rq != 0) {
		isr = irqVector[unmapTbl[rq] + 24];
		if (isr) {
			(*(isr))();
		}
		return;
	}
	
	rq = DMARQ & 0x10;
	if (rq != 0) {
		isr = irqVector[36];
		if (isr) {
			(*(isr))();
		}
		return;
	}
}
