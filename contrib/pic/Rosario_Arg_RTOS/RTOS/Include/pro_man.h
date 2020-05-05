/**********************************************************************/
/*                                                                    */
/* File name: pro_man.h                                               */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v2.10                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*                                                                    */
/* Purpose:   Definition of all the types, defines and functions      */
/*            used by the process manager.                            */
/*                                                                    */
/* Distribution: This file is part of PICos18.                        */
/*            PICos18 is free software; you can redistribute it       */
/*            and/or modify it under the terms of the GNU General     */
/*            Public License as published by the Free Software        */
/*            Foundation; either version 2, or (at your option)       */
/*            any later version.                                      */
/*                                                                    */
/*            PICos18 is distributed in the hope that it will be      */
/*            useful, but WITHOUT ANY WARRANTY; without even the      */
/*            implied warranty of MERCHANTABILITY or FITNESS FOR A    */
/*            PARTICULAR PURPOSE.  See the GNU General Public         */
/*            License for more details.                               */
/*                                                                    */
/*            You should have received a copy of the GNU General      */
/*            Public License along with gpsim; see the file           */
/*            COPYING.txt. If not, write to the Free Software         */
/*            Foundation, 59 Temple Place - Suite 330,                */
/*            Boston, MA 02111-1307, USA.                             */
/*                                                                    */
/*          > A special exception to the GPL can be applied should    */
/*            you wish to distribute a combined work that includes    */
/*            PICos18, without being obliged to provide the source    */
/*            code for any proprietary components.                    */
/*                                                                    */
/* History:                                                           */
/*   2002/09/09  [XM] Create this file.                               */
/*   2002/11/11  [XM] Because kernel and services are not             */
/*                    interruptible, add calls to CGI & SGI macros.   */
/*   2003/01/10  [XM] Add #include <pic....h> directive.              */
/*   2003/01/26  [XM] Replace fixed SECTIONS by relocatable SECTIONS. */
/*   2003/02/02  [XM] Add DeclareRAM(RamStartAddress) macro.          */
/*   2003/02/12  [XM] Change CGI/SGI to allow services call from ISR. */
/*   2003/05/10  [XM] Added STATUS register storage in SAVE_TASK_CTX. */
/*                    Added BSR    register storage in CGI.           */
/*   2003/08/03  [XM] Remove the #include <p18f452.h>.                */
/*                                                                    */
/**********************************************************************/


#ifndef _PRO_MAN_H_
#define _PRE_MAN_H_

/***********************************************************************
 * ------------------------ Type definition ----------------------------
 **********************************************************************/
typedef rom const struct _rom_desc_tsk
{
	unsigned char prioinit;
	unsigned char *stackAddr;
	void (*addr_ROM)(void);
	unsigned char tskstate;
	unsigned char tskid;
	unsigned int  stksize;
} rom_desc_tsk;

typedef unsigned char TaskType;
typedef unsigned char *TaskRefType;
typedef unsigned char TaskStateType;
typedef unsigned char *TaskStateRefType;

typedef unsigned char StatusType;
typedef unsigned char AppModeType;

typedef unsigned int  ResourceType;

typedef struct _Resource {
  unsigned char  priority;
  unsigned char Taskprio;
  unsigned char lock;
} Resource; 

/***********************************************************************
 * ------------------------- Global variables --------------------------
 **********************************************************************/
extern char id_tsk_run;
extern void _sched(void);
extern void _parser_organize(void);

/***********************************************************************
 * ----------------------------- Defines -------------------------------
 **********************************************************************/
#define TASK(TaskName) void TaskName(void)
#define DeclareTask(TaskIdentifier)	extern TASK(TaskIdentifier)

#define SAVE_TASK_CTX(stack_low, stack_high)                           \
{                                                                      \
    /* Disable global interrupt. */                                    \
	_asm                                                               \
        bcf     INTCON, 6, 0                                           \
        movff   STATUS, PREINC1                                        \
        movff   WREG, PREINC1                                          \
	_endasm	                                                           \
    /* Store the necessary registers to the stack. */                  \
	_asm                                                               \
        movff   BSR, PREINC1                                           \
        movff   FSR2L, PREINC1                                         \
        movff   FSR2H, PREINC1                                         \
        movff   FSR0L, PREINC1                                         \
        movff   FSR0H, PREINC1                                         \
        movff   TBLPTRU, PREINC1                                       \
        movff   TBLPTRH, PREINC1                                       \
        movff   TBLPTRL, PREINC1                                       \
        movff   TABLAT, PREINC1                                        \
        movff   PRODH, PREINC1                                         \
        movff   PRODL, PREINC1                                         \
    _endasm                                                            \
                                                                       \
    /* Store the .tempdata and MATH_DATA areas. */                     \
    _asm                                                               \
        movlw   TEMP_SIZE+1                                            \
        clrf    FSR0L, 0                                               \
        clrf    FSR0H, 0                                               \
    _endasm                                                            \
        while (WREG--)                                                 \
        {                                                              \
          _asm                                                         \
            movff   POSTINC0, PREINC1                                  \
          _endasm                                                      \
        }                                                              \
                                                                       \
    /* Store the HW stack area. */                                     \
    _asm                                                               \
        movff   STKPTR, FSR0L                                          \
    _endasm                                                            \
        while (STKPTR > 0)                                             \
        {                                                              \
          _asm                                                         \
            movff   TOSL, PREINC1                                      \
            movff   TOSH, PREINC1                                      \
            movff   TOSU, PREINC1                                      \
            pop                                                        \
          _endasm                                                      \
        }                                                              \
                                                                       \
    /* Store the number of addresses on the HW stack */                \
    _asm                                                               \
        movff   FSR0L, PREINC1                                         \
        movf    PREINC1, 1, 0                                          \
    _endasm                                                            \
                                                                       \
    /* Store the SW stack addr. */                                     \
    _asm                                                               \
        movff   stack_low, FSR0L                                       \
        movff   stack_high, FSR0H                                      \
        movff   FSR1L, POSTINC0                                        \
        movff   FSR1H, POSTINC0                                        \
    _endasm                                                            \
}

#define SCHEDULE            _asm	goto _sched	_endasm
#define ORGANIZE            _asm    goto _parser_organize _endasm
/* SCHEDULE is called by _restore_start_adr */
#define RESTART                                                        \
    _asm                                                               \
        bcf     INTCON, 6, 0                                           \
        goto    _restore_start_adr                                     \
    _endasm

#define KERNEL      0x00
#define USER        0x01
#define ISR         0x02
#define SERVICES    0x04


/* -------- Error returned by OS services --------- */ 
#define E_OK            0x00
#define E_OS_ACCESS     0x01
#define E_OS_CALLEVEL   0x02
#define E_OS_ID         0x03
#define E_OS_LIMIT      0x04
#define E_OS_NOFUNC     0x05
#define E_OS_RESOURCE   0x06
#define E_OS_STATE      0x07
#define E_OS_VALUE      0x08

#define INVALID_TASK    0x10

/* --------------- General defines ---------------- */
#ifndef FALSE
#define FALSE		0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#define AUTO            0x00
#define DISABLE         0x00
#define ENABLE          0x01
#define MIN_TASK_NB     0x00
#define MAX_TASK_NB     0x10

/* ----------------- Task states ------------------ */
#define SUSPENDED       0x00
#define READY           0x20
#define RUNNING         0x40
#define WAITING         0x80

/* ------------- Task mask definitions ------------ */
#define MASK_STATE        0xE0
#define MASK_ID           0x0F
#define MASK_ACTIV        0xF0
#define MASK_PRIO         0x0F

/***********************************************************************
 * ---------------------- Function prototypes --------------------------
 **********************************************************************/
void Save_STKPTR(void);

StatusType SetPriority (unsigned char new_prio, TaskType TaskID);
StatusType GetPriority (unsigned char *the_prio, TaskType TaskID);
StatusType GetTaskID (TaskRefType TaskID);
StatusType GetTaskState (TaskType TaskID, TaskStateRefType State); 
StatusType ActivateTask (TaskType TaskID);
StatusType TerminateTask (void);
StatusType ChainTask (TaskType TaskID);
StatusType Schedule(void); 
StatusType GetResource(ResourceType ID);
StatusType ReleaseResource(ResourceType ID);

AppModeType GetActiveApplicationMode(void);
void StartOS(AppModeType Mode);
void ShutdownOS(StatusType Error);

#endif /* _PRO_MAN_H */


/* End of File : pro_man.h */
