/**
 * main.c
 * 
 * Test routines for the TTA scheduler on the ATMega2560 microcontroller.
 */

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tta.h"
#include "joystick/joystick.h"
#include "uart/uart.h"
#include "message/message.h"
#include "button/button.h"
#include "servo/servo.h"
#include <avr/io.h>
#include <avr/interrupt.h>

typedef enum movement_mode{
	CRUISE = 0,
	STAND_STILL = 1
} MOVEMENT_MODE;

static MOVEMENT_MODE CurrentMode;

void switch_mode_task(void* param_ptr){
	DDRB = 0xFF;
	PORTB = 0xFF;
	
	CurrentMode = (CurrentMode+1) % 2;
	
	PORTB = 0x00;
}

/*
 * There are two types of movement:
 *		1) Forward/backward (right and left wheels have the same value)
 *		2) Turn (right and left wheels have the opposite values)
 * We can only get into the forward/backward state if two conditions are met:
 *		1) The robot is in "Cruise" mode
 *		2) The joystick is pointed more up/down that it is pointed to the side
 *		In other words, the y value offset is greater than the x value offset.
 */
void send_roomba_joystick_task(void* param_ptr){
	DDRB = 0xFF;
	PORTB = 0x00;
	
	CmdMoveRoombaArgs_t args;
	
	int16_t x_value = ((int16_t) (roomba_joystick.x_value)) - 127;
	int16_t y_value = ((int16_t) (roomba_joystick.y_value)) - 127;
	
	// Deadband
	if (abs(x_value) < 50 && abs(y_value) < 50) {
		args.left = 0;
		args.right = 0;
	}
	
	else {
		if (CurrentMode == CRUISE && abs(y_value) >= abs(x_value)){
			// Mode when the Roomba moves forward or backward
			//args.left = (y_value * -7) / 2;
			args.right = (y_value * -5) / 2;
			args.left = args.right;
		} else {
			// Mode when the Roomba can only turn
			//args.left = x_value * 7 / 2;
			args.right = (x_value * 5) / 2;
			args.left = -args.right;
		}
	}
	
	char* msgBuf;
	int len = CmdMoveRoomba_encode(&msgBuf, &args);

	// Turn on a light if we get an error
	if (len == -1){
		PORTB = 0xFF;
	}

	for (int i = 0; i < len; i++)
		uart_putchar(UART_1, msgBuf[i]);

	free(msgBuf);
}

/*
The main function of the program. Will call a test function,
which sets up and runs a test.
*/
int main() {
	joystick_init(&servo_joystick, PF0, PF1);
	joystick_init(&roomba_joystick, PF6, PF7);
	button_init();
	uart_init(UART_1, UART_9600);
	
	Scheduler_Init();
	
	Scheduler_StartPeriodicTask(1, 25, read_joystick_task, &servo_joystick);
	Scheduler_StartPeriodicTask(6, 25, read_joystick_task, &roomba_joystick);
	Scheduler_StartPeriodicTask(12, 100, send_roomba_joystick_task, NULL);
	Scheduler_StartPeriodicTask(77, 100, read_laser_button_task, NULL);
	Scheduler_StartPeriodicTask(44, 100, Task_UpdateServo, NULL);

	Scheduler_StartPeriodicTask(30000,30000,switch_mode_task,NULL);
	
	Scheduler_Start();
}