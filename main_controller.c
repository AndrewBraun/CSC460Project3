/**
 * main.c
 * 
 * Test routines for the TTA scheduler on the ATMega2560 microcontroller.
 */

#include <string.h>
#include "tta.h"
#include "joystick/joystick.h"
#include "button/button.h"
#include <avr/io.h>
#include <avr/interrupt.h>

typedef enum movement_mode{
	CRUISE = 0,
	STAND_STILL = 1
} MOVEMENT_MODE;

static MOVEMENT_MODE CurrentMode;

Joystick servo_joystick, roomba_joystick;

void switch_mode_task(void* param_ptr){
	CurrentMode = (CurrentMode+1) % 2;
}

/*
The main function of the program. Will call a test function,
which sets up and runs a test.
*/
int main() {
	/*
	joystick_init(&servo_joystick, 0x00, 0x01);
	joystick_init(&roomba_joystick, 0x04, 0x08);
	
	Scheduler_Init();
	
	Scheduler_StartPeriodicTask(30000,30000,switch_mode_task(),NULL);
	
	Scheduler_Start();
	*/
	while(1);
}
