/*
 * servo.c
 *
 * Created: 2019-04-01 4:10:44 PM
 *  Author: Andrew
 */
#include <stdio.h>
#include "servo.h"

void servo_init(Servo* servo, uint8_t pin){
	servo->pin = pin;
}

void update_servo(Servo* servo, uint8_t new_period){
	
}

void servo_change_state(Servo* servo){
	
}