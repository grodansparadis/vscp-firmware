/**********************************************************************/
/*                                                                    */
/* File name: even_man.c                                              */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v2.10                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*                                                                    */
/* Purpose:   The event manager is in charge of setting and clearing  */
/*            the events waited by the tasks.                         */
/*            2 areas are used to manage the arrival of events and    */
/*            the fact that some tasks are waiting for these events.  */
/*              - EVENT ram area : for events set by tasks or alarms  */
/*              - WAIT  ram area : for events waited by tasks         */
/*            The job of the event manager is to take care of task    */
/*            activation in case of any similarity inbetween events   */
/*            set and events waited.                                  */
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
/* 2005/10/29 [XM]  Added a critical section in WaitEvent (GIEL = 0). */
/* 2006/05/17 [XM]  Removed the "temp" variable.                      */
/*                                                                    */
/**********************************************************************/

#include "device.h"

/**********************************************************************
 * Shared variables defined in other section.
 **********************************************************************/
extern TaskType tsk_1_state_ID;
extern TaskType tsk_1_activ_prio;
extern char     id_tsk_run;

/**********************************************************************
 * RAM area of the events set by the tasks or alarms.
 **********************************************************************/
#pragma		udata	EVENT_MASK=0x0080
EventMaskType event_1;
EventMaskType event_2;
EventMaskType event_3;
EventMaskType event_4;
EventMaskType event_5;
EventMaskType event_6;
EventMaskType event_7;
EventMaskType event_8;
EventMaskType event_9;
EventMaskType event_10;
EventMaskType event_11;
EventMaskType event_12;
EventMaskType event_13;
EventMaskType event_14;
EventMaskType event_15;
EventMaskType event_16;


/**********************************************************************
 * RAM area of the events waited by the tasks.
 **********************************************************************/
#pragma		udata	WAIT_MASK=0x0090
EventMaskType wait_1;
EventMaskType wait_2;
EventMaskType wait_3;
EventMaskType wait_4;
EventMaskType wait_5;
EventMaskType wait_6;
EventMaskType wait_7;
EventMaskType wait_8;
EventMaskType wait_9;
EventMaskType wait_10;
EventMaskType wait_11;
EventMaskType wait_12;
EventMaskType wait_13;
EventMaskType wait_14;
EventMaskType wait_15;
EventMaskType wait_16;

/**********************************************************************
 * ROM area of the event manager.
 **********************************************************************/
#pragma		code	KERNEL_ROM

/**********************************************************************
 * Set an event in the EVENT ram area of a task.
 * The event is reset if the task is already waiting for this event 
 * (task set into READY state).
 *
 * @param TaskID     IN  Reference of the task
 * @param Mask       IN  Bitmask of the event to set 
 * @return status    E_OK if no waiting task
 *                   E_OS_STATE if task suspended
 *                   E_OS_ID if ID is not correct
 *                   no return value otherwise
 **********************************************************************/
StatusType SetEvent(TaskType TaskID, EventMaskType Mask)
{
  unsigned char i;
  TaskRefType ptr_task;

  ptr_task = (TaskRefType)&tsk_1_state_ID;

  for (i = 0; i < MAX_TASK_NB; i++, ptr_task++)
  {
    if ( (*ptr_task & MASK_ID) == TaskID )
    {
      if ((*ptr_task & READY) == SUSPENDED)
      {
        return (E_OS_STATE);
      }
      ptr_task = &event_1 + i;
      *ptr_task |= Mask;
      ptr_task = &wait_1 + i;

      /* Not any task waiting for at least one event */
      if ( (*ptr_task & Mask) == 0 )
      {
        return (E_OK);
      }
      else
      {
	    ptr_task = (TaskRefType)&tsk_1_state_ID + i;
        *ptr_task &= MASK_ID;
        *ptr_task |= READY;
        Schedule();
        return (E_OK);
      }
    } 
  }

  if (i == MAX_TASK_NB)
    return (E_OS_ID);

  return (E_OK);
}

/**********************************************************************
 * Clear an event in the EVENT ram area and the WAIT ram area of the
 * calling task.
 * Should be called by a task just after the waitevent service call.
 *
 * @param Mask       IN  Bitmask of the event to set 
 * @return status    Always E_OK
 **********************************************************************/
StatusType ClearEvent(EventMaskType Mask)
{
  unsigned char i;
  TaskRefType ptr_task;

  ptr_task = (TaskRefType)&tsk_1_state_ID;

  for (i = 0; i < MAX_TASK_NB; i++, ptr_task++)
  {
    if ( (*ptr_task & MASK_ID) == id_tsk_run )
    {
      ptr_task = &event_1 + i;
      *ptr_task &= ~Mask;
      ptr_task = &wait_1 + i;
      *ptr_task &= ~Mask;
      return (E_OK);
    }
  }

  return (E_OK);
}

/**********************************************************************
 * Get all the event (byte value)from the EVENT ram area of a task.
 * Allow the task to guess from which event it is waked up.
 *
 * @param TaskID     IN  Reference of the task
 * @param Mask       IN  Reference of the event to get 
 * @return status    E_OS_STATE if task suspended
 *                   E_OS_ID if ID is not correct
 *                   E_OK otherwise
 **********************************************************************/
StatusType GetEvent(TaskType TaskID, EventMaskRefType Mask)
{
  unsigned char i;
  TaskRefType ptr_task;

  ptr_task = (TaskRefType)&tsk_1_state_ID;
    
  for (i = 0; i < MAX_TASK_NB; i++, ptr_task++)
  {
    if ( (*ptr_task & MASK_ID) == TaskID )
    {
      if ((*ptr_task & MASK_STATE) == SUSPENDED)
        return (E_OS_STATE);
      ptr_task = &event_1 + i;
      *Mask = (EventMaskType)(*ptr_task);
      return (E_OK);
    }
  }

  if (i == MAX_TASK_NB)
    return (E_OS_ID);

  return (E_OK);
}

/**********************************************************************
 * Set an event in the WAIT ram area of a task.
 * If the same event is set in the EVENT ram area, the function 
 * immediatly returns. If not the task is set in WAITING state.
 *
 * @param Mask       IN  Bitmask of the event to wait 
 * @return status    E_OK if the event exists
 *                   E_OS_ID if ID is not correct 
 *                   no return value otherwise
 **********************************************************************/
StatusType WaitEvent(EventMaskType Mask)
{
  unsigned char i;
  TaskRefType ptr_task;

  ptr_task = (TaskRefType)&tsk_1_state_ID; 

  for (i = 0; i < MAX_TASK_NB; i++, ptr_task++)
  {
    if ( (*ptr_task & MASK_ID) == id_tsk_run )
    {
      INTCONbits.GIEL = 0;
      ptr_task = &wait_1 + i;
      *ptr_task = Mask;
      ptr_task = &event_1 + i;

      /* Not any event waited present */
      if ( (*ptr_task & Mask) == 0 )
      {
        ptr_task = (TaskRefType)&tsk_1_state_ID + i;
        *ptr_task += WAITING;
        Schedule();
      }
      INTCONbits.GIEL = 1;
      return (E_OK);
    }
  }

  if (i == MAX_TASK_NB)
    return (E_OS_ID);

  return (E_OK);
}


/* End of File : even_man.c */
