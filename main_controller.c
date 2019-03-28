/**
 * main.c
 * 
 * Test routines for the TTA scheduler on the ATMega2560 microcontroller.
 */

#include <string.h>
#include "tta.h"
#include "joystick/joystick.h"
#include "uart/uart.h"
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

void send_message_task(void* param_ptr){
	
	DDRB = 0xFF;
	PORTB = 0xFF;
	
	uart_putchar_1('T');
	uart_putchar_1('e');
	uart_putchar_1('s');
	uart_putchar_1('t');
	uart_putchar_1('\n');
	
	PORTB = 0x00;
}

/*
The main function of the program. Will call a test function,
which sets up and runs a test.
*/
int main() {
	//joystick_init(&servo_joystick, 0x00, 0x01);
	//joystick_init(&roomba_joystick, 0x04, 0x08);
	uart_init(UART_9600);
	
	Scheduler_Init();
	
	//Scheduler_StartPeriodicTask(30000,30000,switch_mode_task,NULL);
	Scheduler_StartPeriodicTask(0, 1000, send_message_task, NULL);
	
	Scheduler_Start();
}