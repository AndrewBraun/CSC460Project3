/**
 * main.c
 * 
 * Test routines for the TTA scheduler on the ATMega2560 microcontroller.
 */

#include <string.h>
#include "tta.h"
#include "LED_Test.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void Ping(void* param_ptr){
	init_LED_B5();
	enable_LED(LED_B5_GREEN);
	for(int x = 0; x < 1000; x++);
	disable_LEDs();
}

void Pong(void* param_ptr){
	init_LED_B2();
	enable_LED(LED_B2_GREEN);
	for(int x = 0; x < 1000; x++);
	disable_LEDs();
}

// Basic test that every 5 seconds turns on one of the wires temporarily.
void test1(){
	Scheduler_Init();
	
	Scheduler_StartPeriodicTask(0, 10, Ping, NULL);
	Scheduler_StartPeriodicTask(5, 10, Pong, NULL);
	
	Scheduler_Start();
}

// Basic test that adds two sporadic tests: one at the start and one 25 ms after start.
void test2(){
	Scheduler_Init();
	
	Scheduler_StartPeriodicTask(0, 10, Ping, NULL);
	Scheduler_AddSporadicTask(25, 2, Pong, NULL);
	Scheduler_AddSporadicTask(10, 2, Pong, NULL);
	
	Scheduler_Start();
}

/**
 * Long periodic task (should run for >1ms).
 */
void LongPeriodicTask(void* arg){
	init_LED_B5();
	enable_LED(LED_B5_GREEN);
	for (unsigned int x = 0; x < 64000; x++);
	disable_LEDs();

    PORTB &= ~(1 << PB0);
}

/**
 * Long periodic tasks (>1ms) should not affect our timer values.
 */
void Test3(void){
	Scheduler_Init();
	
	Scheduler_StartPeriodicTask(0, 60, LongPeriodicTask, NULL);
	
	Scheduler_Start();
}

/*
Task that blinks depending on the value passed in to the param argument.
The param passed is an array of two ints.
First blinks the first int's number of times.
Then pauses and blinks for the second int's number of times.
*/
void blink_for_count(void* param_arg){
	init_LED_B5();
	
	for (int i = 0; i < *(((int*)param_arg)); i++){
		enable_LED(LED_B5_GREEN);
		for(int x = 0; x < 100; x++);
		disable_LEDs();
		for(int x = 0; x < 100; x++);
	}
	
	for(int x = 0; x < 1000; x++);
	
	for (int i = 0; i < *(((int*)param_arg+1)); i++){
		enable_LED(LED_B5_GREEN);
		for(int x = 0; x < 100; x++);
		disable_LEDs();
		for(int x = 0; x < 100; x++);
	}
}

/*
Tests passing parameters to a periodic task.
*/
void test4(){
	Scheduler_Init();
	
	int blink_count[] = {1,3};
	void* count_ptr = &blink_count;
	Scheduler_StartPeriodicTask(0,100,blink_for_count, count_ptr);
	
	Scheduler_Start();
}

/*
Tests passing parameters to a sporadic task.
*/
void test5(){
	Scheduler_Init();
	
	int blink_count[] = {1,3};
	void* count_ptr = &blink_count;
	Scheduler_AddSporadicTask(100, 50, blink_for_count,count_ptr);
		
	Scheduler_Start();
}

/*
Blinks and then changes the task's period between 10 and 20 ms.
*/
void blink_change_period(void* param_arg){
	static int period = 10;
	
	init_LED_B5();
	enable_LED(LED_B5_GREEN);
	for(int x = 0; x < 1000; x++);
	disable_LEDs();
	
	// Toggle the period between 10 and 20 seconds.
	period = (period+10) % 30;
	if (!period)
		period = 10;
	Set_Task_Period(0, period);
}

// Tests changing a task's period.
void test6(){
	Scheduler_Init();
		
	Scheduler_StartPeriodicTask(10,10,blink_change_period,NULL);
		
	Scheduler_Start();
}

void test7(){
	Scheduler_Init();
	
	Scheduler_StartPeriodicTask(0, 60, LongPeriodicTask, NULL);
	Scheduler_AddSporadicTask(0, 50, Pong, NULL);
	
	Scheduler_Start();
}

/*
The main function of the program. Will call a test function,
which sets up and runs a test.
*/
int main() {
	test7();
}
