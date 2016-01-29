/**********************************************************************/
/*                                                                    */
/* File name: alarm.c                                                 */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v2.10                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*            NIELSEN  Peter  [PN]                   pnielsen@tuug.fi */
/*                                                                    */
/* Purpose:   The alarm manager is in charge of setting the           */
/*            alarm-timers or updating the alarm-counter from         */
/*            application layer.                                      */
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
/* 2005/04/01 [XM]  Suppress "kernelState |= SERVICES;" at the end of */
/*                  the file to avoid a systematic switch context.    */
/* 2006/01/18 [XM]  Removed functions like _SetEvent.                 */
/*                  Completed IncCounter and GetCounter functions.    */
/*                  Added "kernelState |= SERVICES;" in AddOneTick(). */
/* 2007/01/01 [PN]  Improved alarm accuracy in TMR0 ISR.              */
/*                                                                    */
/**********************************************************************/

#include "device.h"


/**********************************************************************
 * Shared variables defined in other section.
 **********************************************************************/
extern char id_tsk_run;
extern unsigned char stack_low, stack_high;
extern char kernelState;
extern TaskType tsk_1_state_ID;
extern EventMaskType event_1;
extern EventMaskType wait_1;

extern Counter Counter_list[];
extern Counter Counter_kernel;
extern AlarmObject Alarm_list[];
extern unsigned char ALARMNUMBER;
extern unsigned char COUNTERNUMBER;
extern unsigned long global_counter;

union Timers Tmr0;

/**********************************************************************
 * ROM area of the alarm manager.
 **********************************************************************/
#pragma		code	KERNEL_ROM

/**********************************************************************
 * Program an alarm in relative mode with a number of tick to run from 
 * the current count value.
 *
 * @param ID         IN  ID of the alarm to update 
 * @param increment  IN  Number of ticks to run
 * @param cycle      IN  Number of cycle to process 
 * @return Status    E_OK if the alarm is not used
 *                   E_OS_STATE if in USED mode
 *                   E_OS_ID if ID is not correct
 **********************************************************************/
StatusType SetRelAlarm(AlarmType ID, TickType increment, TickType cycle)
{
  if (ID >= ALARMNUMBER)
    return (E_OS_ID);
    
  if (Alarm_list[ID].State == ON)
    return (E_OS_STATE);

  if ((increment > Alarm_list[ID].ptrCounter->Base.maxAllowedValue))
    return (E_OS_VALUE);

  if ((cycle < Alarm_list[ID].ptrCounter->Base.minCycle) || 
      (cycle > Alarm_list[ID].ptrCounter->Base.maxAllowedValue))
    return (E_OS_VALUE);

  Alarm_list[ID].AlarmValue = Alarm_list[ID].ptrCounter->CounterValue + \
                              increment;
  Alarm_list[ID].Cycle      = cycle;
  Alarm_list[ID].State      = ON;
  return (E_OK);
}

/**********************************************************************
 * Program an alarm in absolute mode with a number of tick to run from 
 * the start count value.
 *
 * @param ID         IN  ID of the alarm to update 
 * @param start      IN  Nb of ticks to run from start
 * @param cycle      IN  Number of cycle to process 
 * @return Status    E_OK if the alarm is not used
 *                   E_OS_STATE if in used mode
 *                   E_OS_ID if ID is not correct
 **********************************************************************/
StatusType SetAbsAlarm(AlarmType ID, TickType start, TickType cycle)
{
  if (ID >= ALARMNUMBER)
    return (E_OS_ID);
    
  if (Alarm_list[ID].State == ON)
    return (E_OS_STATE);

  if ((start > Alarm_list[ID].ptrCounter->Base.maxAllowedValue))
    return (E_OS_VALUE);

  if ((cycle < Alarm_list[ID].ptrCounter->Base.minCycle) || 
      (cycle > Alarm_list[ID].ptrCounter->Base.maxAllowedValue))
    return (E_OS_VALUE);

  Alarm_list[ID].AlarmValue = start;
  Alarm_list[ID].Cycle      = cycle;
  Alarm_list[ID].State      = ON;
  return (E_OK);
}

/**********************************************************************
 * Give the number of tick to run to set the alarm.
 *
 * @param ID         IN  ID of the alarm to update 
 * @param Tick       IN  Reference of tick number
 * @return Status    E_OK if the alarm is in USED mode
 *                   E_OS_NOFUNC if not used
 *                   E_OS_ID if ID is not correct
 **********************************************************************/
StatusType GetAlarm(AlarmType ID, TickRefType Tick)
{
  if (ID >= ALARMNUMBER)
    return (E_OS_ID);
    
  if (Alarm_list[ID].State == OFF)
    return (E_OS_NOFUNC);

  *Tick = Alarm_list[ID].AlarmValue - \
          Alarm_list[ID].ptrCounter->CounterValue;
  return (E_OK);
}

/**********************************************************************
 * Set the alarm in UNUSED mode.
 *
 * @param ID         IN  ID of the alarm to update
 * @return Status    E_OK if the alarm is in USE mode
 *                   E_OS_NOFUNC if not used
 *                   E_OS_ID if ID is not correct
 **********************************************************************/
StatusType CancelAlarm(AlarmType ID)
{
  if (ID >= ALARMNUMBER)
    return (E_OS_ID);
    
  if (Alarm_list[ID].State == OFF)
    return (E_OS_NOFUNC);

  Alarm_list[ID].State = OFF;
  return (E_OK);
}

/*********************************************************************
 * Give the info relative to Alarm Base.
 *
 * @param ID         IN  ID of the alarm to update
 * @param Info       IN  Reference to the alarm base 
 * @return Status    E_OK if the ID is correct
 *                   E_OS_ID if not
 *********************************************************************/
StatusType GetAlarmBase(AlarmType ID, AlarmBaseRefType Info)
{
  if (ID >= ALARMNUMBER)
    return (E_OS_ID);
    
  Info = &(Alarm_list[ID].ptrCounter->Base);
  return (E_OK);
}

/*********************************************************************
 * Increment the counter in case of alarm with COUNTER mode.
 *
 * @param ID         IN  ID of the alarm to update
 * @return Status    E_OK if the ID is correct
 *                   E_OS_ACCESS if counter is OFF
 *                   E_OS_ID if ID is not correct
 *********************************************************************/
StatusType IncCounter(AlarmType ID)
{
  if (ID >= ALARMNUMBER)
    return (E_OS_ID);

  if (Alarm_list[ID].State == OFF)
    return(E_OS_ACCESS);

  Alarm_list[ID].ptrCounter->CounterValue++;
  if (Alarm_list[ID].ptrCounter->CounterValue == 
      Alarm_list[ID].AlarmValue)
  {
    if (Alarm_list[ID].Cycle == 0)
      Alarm_list[ID].State = OFF;
    else
    {
      Alarm_list[ID].AlarmValue = \
        Alarm_list[ID].ptrCounter->CounterValue + \
        Alarm_list[ID].Cycle;
    }
    if (Alarm_list[ID].EventToPost != 0)
      SetEvent(Alarm_list[ID].TaskID2Activate, 
               Alarm_list[ID].EventToPost);
    if ((Alarm_list[ID].TaskID2Activate != 0) &&
        (Alarm_list[ID].EventToPost == 0))
      ActivateTask(Alarm_list[ID].TaskID2Activate);
    if (Alarm_list[ID].CallBack != 0)
      Alarm_list[ID].CallBack();
  }
  return (E_OS_ID);
}

/*********************************************************************
 * Give the current number of tick of the alarm in COUNTER mode. 
 * Work also with alarm in TIMER mode.
 *
 * @param ID         IN  ID of the alarm to update 
 * @param Tick       IN  Reference of tick number
 * @return Status    E_OK if ID is correct
 *                   E_OS_ACCESS if counter is OFF
 *                   E_OS_ID if not correct
 *********************************************************************/
StatusType GetCounterValue(AlarmType ID, TickRefType tick)
{
  if (ID >= ALARMNUMBER)
    return (E_OS_ID);

  if (Alarm_list[ID].State == OFF)
    return(E_OS_ACCESS);

  *tick = Alarm_list[ID].ptrCounter->CounterValue;
  return (E_OK);
}

/**********************************************************************
 * Parse the chained list of alarms (timer & counter).
 * Increment timer only.
 * Check if alarm event has to be treated according to AlarmTickValue 
 * and CurrentTickValue.
 * Do it for number of cycle positive or null.
 * Check if a task or event has to be activated. 
 * If a task is waiting the event, clear the event and wait flags 
 * (cf event.c) and set the task READY.
 * Call the SCHEDULER at the end.
 *
 * @return void 
 **********************************************************************/
void AddOneTick (void)
{
  unsigned char index;

  /* To setup 1 tick at 1ms */
  /*TMR0H = Tmr0.bt[1];*/
  /*TMR0L = Tmr0.bt[0];*/
  
  /* To setup 1 tick at 1ms (improved no-drag version) */ 
  union Timers tmr; /* Temp for holding timer value */ 
  tmr.bt[0] = TMR0L; /* Read TMR0 */ 
  tmr.bt[1] = TMR0H; 
  tmr.lt += Tmr0.lt; /* Add offset value to TMR0 */ 
  TMR0H = tmr.bt[1]; 
  TMR0L = tmr.bt[0]; /* Update TMR0 */ 


  INTCONbits.TMR0IF = 0;
  Counter_kernel.CounterValue++;
  global_counter++;
  for (index = 0; index < ALARMNUMBER; index++)
  {
    if (Alarm_list[index].State == OFF)
      continue;

    if (Alarm_list[index].ptrCounter->CounterValue == 
        Alarm_list[index].AlarmValue)
    {
      if (Alarm_list[index].Cycle == 0)
        Alarm_list[index].State = OFF;
      else
      {
        Alarm_list[index].AlarmValue = \
          Alarm_list[index].ptrCounter->CounterValue + \
          Alarm_list[index].Cycle;
      }
      
      if (Alarm_list[index].EventToPost != 0)
        SetEvent(Alarm_list[index].TaskID2Activate, 
                  Alarm_list[index].EventToPost);

      if ((Alarm_list[index].TaskID2Activate != 0) &&
          (Alarm_list[index].EventToPost == 0))
        ActivateTask(Alarm_list[index].TaskID2Activate);

      if (Alarm_list[index].CallBack != 0)
        Alarm_list[index].CallBack();
    }
  }

  kernelState |= SERVICES;
  return;
}


/* End of File : alarm.c */
