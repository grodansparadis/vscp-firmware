/***************************************************************************
   Copyright ARM Limited 1999 - 2000.  All rights reserved.
****************************************************************************

  fiq.c

	$Id: fiq.c,v 1.1 2005/12/19 11:45:57 akhe Exp $

   This file contains the code used by various FIQ handling routines:
   asking for different FIQ's should be done through these routines
   instead of just grabbing them. Thus setups with different FIQ numbers
   shouldn't result in any weird surprises, and installing new handlers
   should be easier.
   	NOTE: Need to define uHALir_UnmaskFiq(), uHALir_MaskFiq()
	and NR_FIQS & MAXFIQNUM to allow uHAL to use FIQs.

****************************************************************************/

#include "platform.h"
#include "uhal.h"
#include "except_h.h"

#if defined(CHAIN_VECTORS) && CHAIN_VECTORS == 1
#include "chain.h"
#endif

#ifndef NR_FIQS
#define NR_FIQS		NR_IRQS
#endif
#ifndef MAXFIQNUM
#define MAXFIQNUM	MAXIRQNUM
#endif

/*
 * Array of fiq handlers.
 */

struct uHALis_IRQ uHALv_FIQVector[NR_FIQS + 1];
int finterrupts[NR_FIQS + 1];
unsigned int uHALiv_FIQSources = 0;

/* Vectors to FIQ functions - initially NULL */
PrVoid uHALp_VectorFIQ = (PrVoid) 0; /* Low level trap handler */
PrVoid uHALp_StartFIQ = (PrVoid) 0; /* Called at start of every FIQ */
PrHandler uHALp_HandleFIQ = (PrHandler) 0; /* High-level Trap handler */
PrPrVoid uHALp_FinishFIQ = (PrPrVoid) 0; /* Called at end of every FIQ */
PrVoid uHALp_SavedFIQ = (PrVoid) 0; /* Previous trap handler (debugger?) */

#if defined(CHAIN_VECTORS) && CHAIN_VECTORS == 1
struct _ChainLink *uHALiv_FIQChainEntry;
#endif

enum uHALe_FIQ_Mode uHALiv_FIQMode;

#if NR_FIQS > 32
#  error Unable to handle more than 32 fiq levels.
#endif

/*
 * Reserved interrupts.  These must NEVER be requested by any driver!
 */
#define	IS_RESERVED_FIQ(fiq)	((fiq) == NR_FIQS)
#define	uHAL_FIQSHARED		0x80    /* Shared interrupt flag */


extern void uHALir_UnmaskFiq(unsigned int fiq);
extern void uHALir_MaskFiq(unsigned int fiq);
extern void uHALir_TrapFIQ(void);

/* Note this routine has no effect if fiq is not in range */
void uHALr_DisableFIQ(unsigned int fiq)
{
    uHALir_MaskFiq(fiq);
    uHALv_FIQVector[fiq].handler = (PrHandler) 0;
	uHALiv_FIQSources &= ~(1 << fiq);
    /* Doesn't disable FIQ, needs to know no other interrupts active */
}


void uHALr_EnableFIQ(unsigned int fiqNum)
{
    uHALiv_FIQSources |= 1 << fiqNum;
    uHALir_UnmaskFiq(fiqNum);
    uHALir_EnableFiq();         /* Make sure the interrupt is enabled */
}


PrVoid uHALir_NewFIQ(PrHandler handler, PrVoid vector)
{
    int spsr;
    int status = 0;
    PrVoid oldVector = 0;

    // If no vector is given, use uHAL's default FIQ Trap routine
    if (0 == vector)
        vector = (PrVoid) uHALir_TrapFIQ;

    // If no handler is given, use the current one.
    if (0 == handler)
	handler = uHALp_HandleFIQ;

    switch (uHALiv_FIQMode)
    {
    case FIQMODE_DIRECT:
	spsr = uHALir_EnterSvcMode();

	// Read the pointer to the old handler
	status = uHALir_ReadVector((void *)FiqV, &oldVector);


	// Install the vector
	status = uHALir_NewVector((void *)FiqV, vector);

	uHALir_ExitSvcMode(spsr);

	if (0 == status)
	    return ((PrVoid) status);
	
	break;

#if defined(CHAIN_VECTORS) && CHAIN_VECTORS == 1
    case FIQMODE_CHAINED:
	if (uHALiv_FIQChainEntry == 0)
	{
	    struct _ChainLink *Chain_link;

		Chain_link = (struct _ChainLink *)uHALr_malloc(sizeof(struct _ChainLink));

	    if (Chain_link != NULL)
		{
	        Chain_link->exec_routine = vector;
	        Chain_link->test_routine = uHALir_TestFIQ;
	        Chain_link->priority = 1;
	        Chain_link->next_element = NULL;
			Chain_link->owner = USER;
			
		    uHALiv_FIQChainEntry = Chainir_Chain_Vectors(ADD_ELEMENT, FIQV, Chain_link);
		    if ((unsigned int)uHALiv_FIQChainEntry == CHAIN_FAIL)
		    {
		        uHALr_free(Chain_link);
		    }
		}
	}
	else
	{
	    uHALiv_FIQChainEntry->exec_routine = vector;
	}
#endif
	break;

    case FIQMODE_INDIRECT:
	/* Nothing to install: we just need to update the variables. */
	break;

    default:
	/* Serious error: FIQMODE is undefined, or defined to an
	 * unsupported value. */
	break;
    }

    //Success, update the Trap handler pointers.
    uHALp_HandleFIQ = handler;
    uHALp_VectorFIQ = vector;

    return (oldVector);
}


/*
 * Routine to check and install requested interrupt. Just sets up logical
 * structures, doesn't alter interrupts at all.
 */
int uHALr_RequestFIQ(unsigned int fiqNum,
                     PrHandler handler, const unsigned char *devname)
{
    struct uHALis_IRQ *action;

    if (fiqNum >= NR_FIQS)
        return -1;
    if (IS_RESERVED_FIQ(fiqNum))
        return -1;
    if (handler == NULL)
        return -1;

    action = &uHALv_FIQVector[fiqNum];

    /* Can't share interrupts (yet) */
    if (action->handler)
        return -1;

    action->handler = handler;
    action->name = devname;
    action->flags = 0;
    action->mask = 0;
    action->next = NULL;

    return 0;
}

/* Call this routine before trying to change the routine attached to a FIQ */
int uHALr_FreeFIQ(unsigned int fiqNum)
{
    if (fiqNum >= NR_FIQS)
    {
        uHALr_printf("Can't free FIQ%d\n", fiqNum);
        return -1;
    }

    if (IS_RESERVED_FIQ(fiqNum))
    {
        uHALr_printf("Can't free reserved FIQ %d\n", fiqNum);
        return -1;
    }

    uHALr_DisableFIQ(fiqNum);

#ifdef DEBUG
    uHALr_printf("free'd FIQ%d\n", fiqNum);
#endif

    return 0;
}

/* Sample routine executed when an unexpected FIQ is received */
void uHALir_UnexpectedFIQ(unsigned int fiq)
{
#if 0
    struct uHALis_IRQ *action;
    int i;

    for (i = 0; i < NR_FIQS; i++)
    {
        action = &uHALv_FIQVector[fiq];
        while (action && action->handler)
        {
            uHALr_printf("[%s:%d] ", action->name, i);
            action = action->next;
        }
    }
    uHALr_printf("\n");
#endif

    uHALr_SetLED(1);
    uHALr_SetLED(2);
    uHALr_SetLED(3);
    uHALr_printf("Unexpected Fast Interrupt = %d\n", fiq);
}


/*
 * Interrupt service routine dispatcher.
 *
 * This is the FIQ handler called when a FIQ trap is taken.
 */
void uHALir_DispatchFIQ(unsigned int flags)
{
    int fiq = 0;
    struct uHALis_IRQ *action;

    // See if there is a flag set, if not then don't do anything
    while (flags != 0)
    {
        /*
         * The following if statements will find the least significant
         * bit that is set.  After this code has executed the flags
         * variable will be shifted left so that the first flag will be
         * bit zero and the fiq variable is the bit position.
         */
        if ((flags & 0xffff) == 0)
        {
            fiq += 16;
            flags >>= 16;
        }

        if ((flags & 0xff) == 0)
        {
            fiq += 8;
            flags >>= 8;
        }

        if ((flags & 0xf) == 0)
        {
            fiq += 4;
            flags >>= 4;
        }

        if ((flags & 0x3) == 0)
        {
            fiq += 2;
            flags >>= 2;
        }

        if ((flags & 0x1) == 0)
        {
            fiq += 1;
            flags >>= 1;
        }

        if (fiq > MAXFIQNUM)
            break;

        // Get the vector and process the interrupt.
        action = &uHALv_FIQVector[fiq];

        if (action->handler == NULL)
        {
            if(uHALiv_FIQMode == FIQMODE_DIRECT) 
	    {
   		/* Only report unexpected IRQs if we have direct control
		 * of the IRQ vector. */
                uHALir_UnexpectedFIQ(fiq);
            }
        }
        else
        {
            do
            {
                // Process multiple handlers if they exist.
                action->handler(fiq);
            }
            while ((action = action->next) != NULL);
        }

        /*
         * As stated above the bit that corresponds to this interrupt
         * is now the LSB of flags.  Therefore we need to clear this bit
         * before going around the loop again.
         */
        flags &= ~1;
    }
}                               /* DispatchFIQ */

/*
 * Interrupt service routine test.
 *
 * This is the FIQ handler called when an FIQ trap is taken
 * to determine if the FIQ is for uHAL.
 */
int uHALir_FlagsTestFIQ(unsigned int flags)
{
    int uhal_int = 2;

    if ((uHALiv_FIQSources & flags) != 0)
    {
        uhal_int = 1;
    }

    return uhal_int;
}

/*
 * Called once on start-up by projects which require one or more special
 * FIQ routines. 
 *	Start is the address of a routine called at the start of every FIQ.
 *	Finish is the address of a routine called at the end of every FIQ.
 *	 If Finish wants to do really low-level stuff (such as context
 *	 switch etc.), the FIQ routine will jump to the returned value
 *	 (if non-zero). 
 *	Trap is the address of the routine if you want to completely
 *	 replace the routine vectored to by FIQ.
 */
void uHALir_DefineFIQ(PrVoid Start, PrPrVoid Finish, PrVoid Trap)
{
    /* These vectors can be zero'd if required */
    uHALp_StartFIQ = Start;
    uHALp_FinishFIQ = Finish;

    /* Only setup the FIQ vector if it isn't zero */
    if (Trap != NULL)
        uHALp_VectorFIQ = Trap;

    /* Call NewFIQ to install the new FIQ */
}

/* Initialise the IRQ environment without installing interrupt handlers. */
enum uHALe_FIQ_Mode uHALir_ResetFIQs(enum uHALe_FIQ_Mode mode)
{
    int i;

    /* If we've been given an explicit mode, use that without asking
     * any questions. */
    if (mode != FIQMODE_NOT_INITIALISED)
    {
	uHALiv_FIQMode = mode;
	return uHALiv_FIQMode; 	
    }
    
    /* Don't allow interrupts to be reset twice. */
    if (uHALiv_FIQMode != FIQMODE_NOT_INITIALISED)
	return uHALiv_FIQMode; 	

    /* Default IRQ mode is to control the IRQ vector directly. */
    uHALiv_FIQMode = FIQMODE_DIRECT;
    
#if defined(CHAIN_VECTORS) && CHAIN_VECTORS == 1
    if ( (Chainir_DebuggerFlags() & (1 << FIQV)) == (1 << FIQV) )
    {
	/* We are in a chaining environment. */
	uHALiv_FIQMode = FIQMODE_CHAINED;
    }
#endif

    /* Mask out all interrupt sources. */
    for (i = 0; i < MAXFIQNUM; i++)
   	{
		if (uHALiv_FIQMode != FIQMODE_CHAINED)
		{
			uHALir_MaskFiq(i);
	    }
        uHALv_FIQVector[i].flags = 0;
        uHALv_FIQVector[i].handler = (PrHandler) 0;
        uHALv_FIQVector[i].next = NULL;
        uHALv_FIQVector[i].name = 0;		
	}	

	return uHALiv_FIQMode; 	

}

/* Called once on start-up to initialise data structures & interrupt routines */
void uHALr_InitFIQ(void)
{
    uHALir_ResetFIQs(FIQMODE_NOT_INITIALISED);
    uHALp_SavedFIQ = uHALir_NewFIQ(uHALir_DispatchFIQ, uHALp_VectorFIQ);
}

// End of file -fiq.c
