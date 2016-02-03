/**********************************************************************/
/*                                                                    */
/* File name: pro_man.c                                               */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v2.09                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*                                                                    */
/* Purpose:   The process manager is in charge of changing or giving  */
/*            the state of the different tasks, and also of starting  */
/*            or stopping the kernel.                                 */
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
/* 2004/09/20 [XM]  Create this file.                                 */
/* 2006/01/18 [XM]  Removed the temp variables (JohnG improvment).    */
/* 2006/07/31 [XM]  Fixed bug in TerminateTask (MASK_ID<->MASK_STATE).*/
/*                                                                    */
/**********************************************************************/

#include "device.h"

/**********************************************************************
 * Shared variables defined in other section.
 **********************************************************************/
extern char id_tsk_run;
extern char kernelState;
extern void _kernel(void);
extern void _restore_start_adr(void);
extern void _restore_now(void);

/**********************************************************************
 * Functions declared in other sections.
 **********************************************************************/
#ifdef	SHUTDOWNHOOK
	extern void ShutdownHook(StatusType Error);
#endif

#ifdef	ERRORHOOK
	extern void ErrorHook(StatusType Error);
#endif

extern Resource Resource_list[];
extern unsigned char RESOURCENUMBER;

unsigned char FSR1H_MAIN;
unsigned char FSR1L_MAIN;
unsigned char appmode;
unsigned char stack_low, stack_high;


/**********************************************************************
 * RAM area of the stack field of a task.
 **********************************************************************/
#pragma		udata	STACK_RAM=0x00A0
TaskType tsk_1_state_ID;
TaskType tsk_2_state_ID;
TaskType tsk_3_state_ID;
TaskType tsk_4_state_ID;
TaskType tsk_5_state_ID;
TaskType tsk_6_state_ID;
TaskType tsk_7_state_ID;
TaskType tsk_8_state_ID;
TaskType tsk_9_state_ID;
TaskType tsk_10_state_ID;
TaskType tsk_11_state_ID;
TaskType tsk_12_state_ID;
TaskType tsk_13_state_ID;
TaskType tsk_14_state_ID;
TaskType tsk_15_state_ID;
TaskType tsk_16_state_ID;

/**********************************************************************
 * RAM area of the state field of the task.
 **********************************************************************/
#pragma		udata	STATE_RAM=0x00B0
TaskType tsk_1_activ_prio;
TaskType tsk_2_activ_prio;
TaskType tsk_3_activ_prio;
TaskType tsk_4_activ_prio;
TaskType tsk_5_activ_prio;
TaskType tsk_6_activ_prio;
TaskType tsk_7_activ_prio;
TaskType tsk_8_activ_prio;
TaskType tsk_9_activ_prio;
TaskType tsk_10_activ_prio;
TaskType tsk_11_activ_prio;
TaskType tsk_12_activ_prio;
TaskType tsk_13_activ_prio;
TaskType tsk_14_activ_prio;
TaskType tsk_15_activ_prio;
TaskType tsk_16_activ_prio;

/**********************************************************************
 * ROM area of the process manager.
 **********************************************************************/
#pragma		code	KERNEL_ROM

/**********************************************************************
 * Modify the priority of a task.
 * Not mandatory by OSEK standart.
 *
 * @param new_prio   IN  New value of the task priority
 * @param TaskID     IN  ID of the task
 * @return Status    E_OK if the task ID is correct
 *                   E_OS_ID otherwise
 **********************************************************************/
StatusType SetPriority (unsigned char new_prio, TaskType TaskID)
{
  unsigned char i;
  TaskRefType ptr_task;

  ptr_task = (TaskRefType)&tsk_1_state_ID; 

  for (i = 0; i < MAX_TASK_NB; i++)
  {
    if ( (*ptr_task & MASK_PRIO) == TaskID )
    {
	  ptr_task   = &tsk_1_activ_prio + i;
	  *ptr_task  = *ptr_task & MASK_ACTIV;
      *ptr_task += new_prio;
      return (E_OK);
    }
    ptr_task++;
  }

  return (E_OS_ID);
}

/**********************************************************************
 * Modify the priority of a task.
 * Not mandatory by OSEK standart.
 *
 * @param new_prio   IN  New value of the task priority
 * @param TaskID     IN  ID of the task
 * @return Status    E_OK if the task ID is correct
 *                   E_OS_ID otherwise
 **********************************************************************/
StatusType GetPriority (unsigned char *the_prio, TaskType TaskID)
{
  unsigned char i;
  TaskRefType ptr_task;

  ptr_task = (TaskRefType)&tsk_1_state_ID; 

  for (i = 0; i < MAX_TASK_NB; i++)
  {
    if ( (*ptr_task & MASK_ID) == TaskID )
    {
	  ptr_task   = &tsk_1_activ_prio + i;
      *the_prio  = *ptr_task & MASK_PRIO;
      return (E_OK);
    }
    ptr_task++;
  }

  return (E_OS_ID);
}

/**********************************************************************
 * Set id_tsk_run into TaskID or INVALID_TASK (0x10) if the ID is out 
 * of range.
 *
 * @param TaskID     IN  Reference of the task
 * @return status    Always E_OK 
 **********************************************************************/
StatusType GetTaskID (TaskRefType TaskID)
{
  if ( (id_tsk_run >= 0) && (id_tsk_run < MAX_TASK_NB) )
    *TaskID = (TaskType)id_tsk_run;
  else
    *TaskID = (TaskType)INVALID_TASK;

  return (E_OK);
}

/**********************************************************************
 * Copy a byte into a buffer.
 *
 * @param TaksID     IN  ID of the task 
 * @param State      OUT Reference of the task state
 * @return Status    E_OK if the task ID is correct
 *                   E_OS_ID otherwise
 **********************************************************************/
StatusType GetTaskState (TaskType TaskID, TaskStateRefType State)
{
  unsigned char i;
  TaskRefType ptr_task;

  ptr_task = (TaskRefType)&tsk_1_state_ID; 

  for (i = 0; i < MAX_TASK_NB; i++)
  {
    if ( (*ptr_task & MASK_ID) == TaskID )
    {
      *State = (TaskStateType)(*ptr_task & MASK_STATE);
      return (E_OK);
    }
    ptr_task++;
  }

  return (E_OS_ID);
}

/**********************************************************************
 * Set the task to the READY state and call the scheduler to jump to 
 * another task if necessary.
 *
 * @param TaskID     IN  ID of the task
 * @return Status    E_OS_ID if ID is not correct
 *                   never return otherwise
 **********************************************************************/
StatusType ActivateTask (TaskType TaskID)
{
  unsigned char i;
  TaskRefType ptr_task;

  ptr_task = (TaskRefType)&tsk_1_state_ID; 

  for (i = 0; i < MAX_TASK_NB; i++)
  {
    if ( (*ptr_task & MASK_ID) == TaskID )
    {
      if ((*ptr_task & MASK_STATE) == SUSPENDED)
      {
        *ptr_task &= MASK_ID;
        *ptr_task += READY;
        Schedule();
      }
      else
      {
        ptr_task   = (TaskRefType)&tsk_1_activ_prio + i;
        if ((*ptr_task & MASK_ACTIV) < MASK_ACTIV)
          *ptr_task += 0x10;
      }
      return (E_OK);
    }
    ptr_task++;
  }

  return (E_OS_ID);
}

/**********************************************************************
 * Allow a task to terminate itself. Cannot terminate another task.
 * To prepare a new activation of the task, we need first to store in
 * stack the start adress of the task
 *
 * @param dest       OUT Destination buffer 
 * @param src        IN  The byte to copy
 * @return Status    E_OK if ID is not correct
 *                   In fact the function never return 
 **********************************************************************/
StatusType TerminateTask (void)
{
  unsigned char i;
  TaskRefType ptr_task;
   
  ptr_task = (TaskRefType)&tsk_1_state_ID;
   
  for (i = 0; i < MAX_TASK_NB; i++)
  { 
    if ( (*ptr_task & MASK_ID) == id_tsk_run )
    {
	  ptr_task     = (TaskRefType)&tsk_1_activ_prio + i;
	  if ((*ptr_task & MASK_STATE) == 0)
	  {
        ptr_task   = (TaskRefType)&tsk_1_state_ID + i;
        *ptr_task &= MASK_ID;
        *ptr_task += SUSPENDED;
        RESTART;
      }
      else
      {
	    *ptr_task -= 0x10;
        RESTART;
	  }
    }
    ptr_task++;
  }
 
  return (E_OK);
}

/**********************************************************************
 * Chain a task by setting the calling task in a SUSPENDED state and 
 * the called taks in the READY state.
 * Call the scheduler to jump the chained task.
 * This doesn't take into account the multiple activation feature, then
 * if a calling task has been activated twice a chaintask will erased 
 * its activation counter (the calling task will be SUSPENDED).
 *
 * @param TaskID     IN  ID of the next task to chain
 * @return Status    E_OK if ID is not correct
 *                   In fact the function never return 
 **********************************************************************/
StatusType ChainTask (TaskType TaskID)
{
  unsigned char i;
  TaskRefType ptr_task;

  ptr_task = (TaskRefType)&tsk_1_state_ID;

  for (i = 0; i < MAX_TASK_NB; i++)
  {
    if ( (*ptr_task & MASK_ID) == id_tsk_run )
    {
      *ptr_task &= MASK_ID;
      *ptr_task += SUSPENDED;
    }
    if ( (*ptr_task & MASK_ID) == TaskID )
    {
      *ptr_task &= MASK_ID;
      *ptr_task += READY;
    }
    ptr_task++;
  }
  
  RESTART;
  return (E_OK);
}

/**********************************************************************
 * Force a scheduler action
 *
 * @return Status    E_OK if a service is called inside an ISR
 *                   or never returns
 **********************************************************************/
StatusType Schedule(void)
{
  INTCONbits.GIEL = 0;
  kernelState |= SERVICES;
  if (kernelState & ISR)
    return (E_OK);
  kernelState &= ~SERVICES;
  if (kernelState & USER)
    SAVE_TASK_CTX(stack_low, stack_high);
  SCHEDULE;
  return (E_OK);
}

/**********************************************************************
 * Return the appmode global variable.
 *
 * @return AppMode   The mode set by application 
 **********************************************************************/
AppModeType GetActiveApplicationMode(void)
{
  return(appmode);
}

/**********************************************************************
 * Jump to the KERNEL code area by setting the appmode global variable. 
 * This service is for call from main routine only.
 * Store at first the return adress in main routine.
 *
 * @param Mode       IN  Mode to set into appmode
 * @return void
 **********************************************************************/
void StartOS(AppModeType Mode)
{
  SAVE_TASK_CTX(FSR1L, FSR1H);
  FSR1L_MAIN = FSR1L;
  FSR1H_MAIN = FSR1H;
  appmode = Mode;
  STKPTR = 0;
  _asm	goto _kernel  _endasm;
}

/**********************************************************************
 * Stop the kernel and task activation after an error occurs. 
 * The function returns to the adress of the main function stored by 
 * the StartOS service.
 *
 * @param error      IN  Last error number detected
 * @return void
 **********************************************************************/
void ShutdownOS(StatusType Error)
{	
#ifdef	SHUTDOWNHOOK
  ShutdownHook(Error);
#endif
  FSR1L = FSR1L_MAIN;
  FSR1H = FSR1H_MAIN;  
  _restore_now();
}

/**********************************************************************
 * Force the kernel to bring into order the list of tasks function to  
 * priority.
 *
 * @param  void
 * @return void
 **********************************************************************/
void Organize(void)
{
  SAVE_TASK_CTX(stack_low, stack_high);
  ORGANIZE;
}

/**********************************************************************
 * Set the task priority to the resource priority. 
 * See the ceiling protocol of the OSEK/VDX standart.
 *
 * @param ID         IN  ID of the resource to be accessed
 * @return Status    E_OS_ACCESS if the resource does not exist
 *                   Elsewise the function never returns 
 **********************************************************************/
StatusType GetResource(ResourceType ID)
{
  if (ID >= RESOURCENUMBER)
    return (E_OS_ID);
  if (Resource_list[ID].lock == 1)
    return E_OS_ACCESS;
  GetPriority(&Resource_list[ID].Taskprio, id_tsk_run);
  SetPriority(Resource_list[ID].priority, id_tsk_run);
  Resource_list[ID].lock = 1;
  Organize();
  return(E_OK);
}

/**********************************************************************
 * Restore the task priority to original value. 
 * See the ceiling protocol of the OSEK/VDX standart.
 *
 * @param ID         IN  ID of the resource to be accessed
 * @return Status    E_OS_ACCESS if the resource does not exist
 *                   Elsewise the function never returns 
 **********************************************************************/
StatusType ReleaseResource(ResourceType ID)
{
  if (ID >= RESOURCENUMBER)
    return (E_OS_ID);
  if (Resource_list[ID].lock == 0)
    return E_OS_ACCESS;
  SetPriority(Resource_list[ID].Taskprio, id_tsk_run);
  Resource_list[ID].lock = 0;
  Organize();
  return(E_OK);
}


/* End of File : pro_man.c */
