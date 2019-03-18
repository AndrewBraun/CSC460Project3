#ifndef TTA_H
#define TTA_H

#include <stdint.h>
// The maximum number of tasks
#define MAXTASKS	8

#define Disable_Interrupt()		asm volatile ("cli"::)
#define Enable_Interrupt()		asm volatile ("sei"::)

typedef void (*voidfuncptr) (void* ptr);      /* pointer to void f(void* ptr) */

/**
 * Initializes the scheduler and starts the underlying scheduler timer (see
 * Schedtimer_Init).
 */
void Scheduler_Init();

/**
 * Schedules a task callback to run periodically while the scheduler is active
 * @param delay: Number of milliseconds before starting the first call to task.
 * @param period: Number of milliseconds between the start of one call to task and 
 * 				  the next.
 * @param task: Pointer to the task to be run.
 * @param state_struct_ptr: Pointer to any persistent data. Will be passed as an argument
 * 							on all calls to task.
 */
void Scheduler_StartPeriodicTask(int16_t delay, uint16_t period, voidfuncptr task, void* state_struct_ptr);

/**
 * Dispatches the next scheduled periodic task. Called from scheduler main loop.
 */
uint16_t Scheduler_DispatchPeriodic();

/**
 * Alters the scheduled period of a running task.
 * @param task_id: Index of task in scheduler task array.
 * @param new_period: New task period in milliseconds.
 */
void Set_Task_Period(uint8_t task_id, int16_t new_period);

/**
 * Adds a sporadic task to the scheduler, which will be run once after a given 
 * delay.
 * @param delay: Time before task is run in milliseconds.
 * @param execution_time: Estimated time to execute the task.
 * @param task: Task callback pointer.
 * @param state_struct_ptr: Pointer to a persistent state for the task. Will be passed
 * 							as an argument on every call.
 */
void Scheduler_AddSporadicTask(int16_t delay, uint16_t execution_time, voidfuncptr task, void* state_struct_ptr);

/**
 * Dispatches the next scheduled sporadic task. Called from scheduler main loop.
 */
void Scheduler_DispatchSporadic();

/**
 * Starts the scheduler. Serves as the main loop of the OS.
 */
void Scheduler_Start();

#endif
