/**********************************************************************/
/*                                                                    */
/* File name: alarm.h                                                 */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v2.10                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
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
/* 2002/09/09 [XM]  Create this file.                                 */
/* 2002/11/02 [XM]  MPLAB-C18 is not able to compute typedef          */
/*                  struct in the same line.                          */
/* 2003/01/26 [XM]  Replace fixed SECTIONS by relocatable SECTIONS.   */
/* 2005/11/12 [XM]  Fixed "AlarmRefType" bug (* missing).             */
/*                                                                    */
/**********************************************************************/

#ifndef _ALARM_H_
#define _ALARM_H_

/***********************************************************************
 * ---------------------------- Defines --------------------------------
 **********************************************************************/
#define USED               0x01
#define ACTIVATETASK       0x02
#define SETEVENT           0x00
#define CLOCKED            0x04
#define NOT_CLOCKED        0x00
#define INFINITE           0x08
#define BASIC              0x00

/***********************************************************************
 * ------------------------ Type definition ----------------------------
 **********************************************************************/
typedef unsigned int   TickType;
typedef unsigned int  *TickRefType;
typedef unsigned char  AlarmType;
typedef unsigned char *AlarmRefType;

typedef struct _AlarmBaseType
{
  TickType maxAllowedValue;        /* equal to PR of TMR1  */
  TickType ticksPerBase;           /* Prescaler            */
  TickType minCycle;               /* inc value for cyclic */
}AlarmBaseType, *AlarmBaseRefType;

typedef struct _Counter
{
  AlarmBaseType Base;
  TickType CounterValue;           /* Courent value of CPT */
  TickType Tick;
}Counter, *RefCounter;

typedef struct _AlarmObject
{
  unsigned char State;             /* ON - OFF */
  TickType      AlarmValue;        /* if == counter => alarm */
  unsigned int  Cycle;             /* cyclic inc value */
  RefCounter    ptrCounter;        /* CPT ref */
  unsigned char TaskID2Activate;
  unsigned int  EventToPost;
  void(*CallBack)(void);
}AlarmObject, *AlarmRefObject;

union Timers {
  unsigned int lt;
  char         bt[2];
  };

/***********************************************************************
 * ------------------------ From alarm.c file --------------------------
 **********************************************************************/
#define DeclareAlarm(a_alarm)	extern AlarmRefObject a_alarm

StatusType SetRelAlarm(AlarmType ID, TickType increment, TickType cycle);
StatusType SetAbsAlarm(AlarmType ID, TickType start, TickType cycle);
StatusType GetAlarm(AlarmType ID, TickRefType Tick);
StatusType CancelAlarm(AlarmType ID);
StatusType GetAlarmBase(AlarmType ID, AlarmBaseRefType Info);
StatusType SetAlarmAction(AlarmType ID, unsigned char Action, unsigned char Type);
StatusType SetAlarmTarget(AlarmType ID, TaskType TaskID);
StatusType IncCounter(AlarmType ID);
StatusType GetCounterValue(AlarmType ID, TickRefType tick);

#define GetAlarmID(AlarmName)     AlarmName.ID

#endif /* _ALARM_H_ */


/* End of File : alarm.h */
