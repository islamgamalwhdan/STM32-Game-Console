/*
 * Scheduler.c
 *
 *  Created on: May 12, 2021
 *      Author: Islam
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "Scheduler.h"

#define MAX_TASKS 5

static TaskType Tasks[MAX_TASKS];

/* Private Functions ------------------------------------------------------------------*/

static int8_t Tsk_GetNumTasks(void)
{
    uint32_t task_count = 0 ;
	for(uint32_t i = 0 ; i< MAX_TASKS ; i++ )
		{
		     if(Tasks[i].Func) task_count++ ;
		}
	return task_count ;
}

/* Public Functions ------------------------------------------------------------------*/

void Add_Task( void(*Ptask)(void) , uint16_t interval )
{
	static uint16_t tsk_cnt =0 ;

	Tasks[tsk_cnt].Func     = Ptask ;
	Tasks[tsk_cnt].Interval = interval ;
	Tasks[tsk_cnt].LastTick = 0 ;
	tsk_cnt++ ;
}

void Delete_Task( void(*Ptask)(void))
{
	for(uint32_t i = 0 ; i < MAX_TASKS ; i++ )
	{
		if(Tasks[i].Func)
		{
			if(Tasks[i].Func == Ptask) Tasks[i].Func = NULL ;
		}
	}
}

void Setup_Scheduler(void)
{

}
void Run_Scheduler(void)
{
	uint32_t tick = HAL_GetTick();    // Get current system tick
	uint32_t NumTasks = Tsk_GetNumTasks() ;

	// Loop through all tasks.  First, run all continuous tasks.  Then,
	// if the number of ticks since the last time the task was run is
	// greater than or equal to the task interval, execute the task.
	for(uint32_t TaskIndex = 0; TaskIndex < NumTasks; TaskIndex++)
	        {
	          if(Tasks[TaskIndex].Interval == 0)
	          {
	            // Run continuous tasks.
	            (Tasks[TaskIndex].Func)();
	          }
	         else if((tick - Tasks[TaskIndex].LastTick) >= Tasks[TaskIndex].Interval)
	          {
	            (Tasks[TaskIndex].Func)();         // Execute Task

	            Tasks[TaskIndex].LastTick = tick;  // Save last tick the task was ran.
	          }
	        }
}
