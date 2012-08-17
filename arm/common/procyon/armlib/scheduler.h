/*! \file scheduler.h \brief Simple Task Scheduler. */
//*****************************************************************************
//
// File Name	: 'scheduler.h'
// Title		: Simple Task Scheduler
// Author		: Pascal Stang - Copyright (C) 2006
// Created		: 2006.05.24
// Revised		: 2006.05.26
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "global.h"

// structures and typedefs
typedef void (*taskfuncptr)(int htask, int systime);
typedef void (*voidFuncPtr)(void);

struct task
{
	int runtime;
	int nrepeat;
	int interval;
	taskfuncptr funcptr;
};

#ifndef TASKLIST_SIZE
#define TASKLIST_SIZE	10
#endif

// functions

// initialize scheduler
void schedulerInit(void);

// run scheduler
//	this function should be called at the system clock tick rate
//	the system time must be passed in systime
void schedulerRun(int systime);

// schedule a task
//	returns a handle to the task (or -1 for failure)
int schedulerAddTask(int runtime, int nrepeat, int interval, taskfuncptr taskfunc);

// remove a scheduled task by handle
int schedulerRemoveTask(int taskhandle);

// remove a scheduled task(s) by function reference
//	NOTE: this will remove ALL tasks referencing the specified function
int schedulerRemoveTaskFunc(taskfuncptr funcptr);

#endif
