/*
 * servo.c
 *
 * Created: 2019-04-01 4:10:44 PM
 *  Author: Andrew
 */
#include <stdio.h>
#include <stdlib.h>
#include "servo.h"
#include "../joystick/joystick.h"
#include "../message/message.h"
#include "../uart/uart.h"

void send_servo_joystick_task(void* param_ptr){
	DDRB = 0xFF;
	PORTB = 0x00;
	
	int16_t x_value = ((int16_t) (servo_joystick.x_value)) - 127;
	int16_t y_value = ((int16_t) (servo_joystick.y_value)) - 127;
	
	int8_t pan_velocity = 0;
	int8_t tilt_velocity = 0;
		
	// Deadband
	if (abs(x_value) >= 50 || abs(y_value) >= 50) {
		pan_velocity = x_value > 0 ? 1 : -1;
		tilt_velocity = y_value > 0 ? 1 : -1;
	}
	
	uart_putchar(UART_1, Cmd_MoveServos);
	uart_putchar(UART_1, pan_velocity);
	uart_putchar(UART_1, tilt_velocity);
}

void servo_init(Servo* servo, uint8_t pin){
	servo->pin = pin;
	servo->position = 90;
	servo->velocity = 0;
}

void Task_UpdateServo(void* servo_arg){
	Servo* servo = (Servo *) servo_arg;
	
}