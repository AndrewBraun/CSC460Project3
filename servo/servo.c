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

// Our pins are 7 and 8
// Credit: https://forum.arduino.cc/index.php?topic=72092.0
// https://www.avrfreaks.net/forum/help-needed-pwm-atmega-2560
// http://sobisource.com/arduino-mega-pwm-pin-and-frequency-timer-control/
void servo_init(Servo* servo, uint8_t pin){
	TCCR3A = 0xFB;
	//TCCR3B = (TCCR3B & 0xF8) | 6;
	TCCR4B = (TCCR4B & 0xF8) | 0x04;
	
	OCR4B = 80;
	OCR4C = 80;
	
	DDRH |= (1 << PH4) | (1 << PH5);
	
	servo->pin = pin;
	servo->position = 90;
	servo->velocity = 0;
}

void Task_UpdateServo(void* servo_arg){
	Servo* servo = (Servo *) servo_arg;
	
	servo->position += servo->velocity;
	if(servo->position < 0) {
		servo->position = 0;
	} else if(servo->position > 4) {
		servo->position = 4;
	}
	
	if (servo->pin == PH4){
		// pin 7
		OCR4B = positions[servo->position];
	} else if (servo->pin == PH5) {
		// pin 8
		OCR4C = positions[servo->position];
	}
}