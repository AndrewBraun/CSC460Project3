/*
 * joystick.h
 *
 * Created: 2019-03-18 3:31:20 PM
 *  Author: Andrew
 */ 

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#define JOYSTICK_ALPHA 0.2

typedef struct joystick{
	uint8_t x_value;
	uint8_t y_value;
	uint8_t x_pin;
	uint8_t y_pin;
} Joystick;

Joystick servo_joystick, roomba_joystick;

void joystick_init(Joystick* joystick, uint8_t x_pin, uint8_t y_pin);

void read_joystick_task(void* joystick_arg);

#endif /* JOYSTICK_H_ */