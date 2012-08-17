/*! \file scheduler.c \brief Simple Task Scheduler. */
//*****************************************************************************
//
// File Name	: 'scheduler.c'
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

#include "global.h"
#include "processor.h"
#include "scheduler.h"

// the task list
struct task TaskList[TASKLIST_SIZE];

// functions
void schedulerInit(void)
{
	int i;
	
	// reset task list
	for(i=0; i<TASKLIST_SIZE; i++)
	{
		// initialize task list
		TaskList[i].runtime = 0;
		TaskList[i].funcptr = 0;
	}
}

void schedulerRun(int systime)
{
	int i;
	
	// locate active task slots
	for(i=0; i<TASKLIST_SIZE; i++)
	{
		if(TaskList[i].funcptr)
		{
			// check systime against runtime
			if(systime >= TaskList[i].runtime)
			{
				// run task, then update it
				TaskList[i].funcptr(i,systime);
				if(TaskList[i].nrepeat)
				{
					TaskList[i].runtime += TaskList[i].interval;
					TaskList[i].nrepeat--;
					// task has expired, remove it now
					if(!TaskList[i].nrepeat)
						schedulerRemoveTask(i);
				}
				else
				{
					// nrepeat==0 means run forever
					TaskList[i].runtime += TaskList[i].interval;
				}
				
			}
		}
	}

	// increment scheduler timer
	//SchedulerTimer++;
}

int schedulerAddTask(int runtime, int nrepeat, int interval, taskfuncptr taskfunc)
{
	int i;
	// locate empty scheduler slot
	for(i=0; i<TASKLIST_SIZE; i++)
	{
		CRITICAL_SECTION_BEGIN;
		if(!TaskList[i].funcptr)
		{
			// setup entry
			TaskList[i].funcptr = taskfunc;
			TaskList[i].runtime = runtime;
			TaskList[i].nrepeat = nrepeat;
			TaskList[i].interval = interval;
			// return task handle
			CRITICAL_SECTION_END;
			return i;
		}
		CRITICAL_SECTION_END;
	}
	// error - no task slots left
	return -1;
}

int schedulerRemoveTask(int taskhandle)
{
	// clear the task entry
	TaskList[taskhandle].runtime = 0;
	TaskList[taskhandle].interval = 0;
	TaskList[taskhandle].nrepeat = 0;
	TaskList[taskhandle].funcptr = 0;
	return taskhandle;
}

int schedulerRemoveTaskFunc(taskfuncptr funcptr)
{
	int i;
	int taskhandle = -1;
	// locate the task
	for(i=0; i<TASKLIST_SIZE; i++)
	{
		// on match, disable the task
		if(TaskList[i].funcptr == funcptr)
			taskhandle = schedulerRemoveTask(i);
	}
	return taskhandle;
}
