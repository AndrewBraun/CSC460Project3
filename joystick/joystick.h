/*
 * joystick.h
 *
 * Created: 2019-03-18 3:31:20 PM
 *  Author: Andrew
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

typedef struct joystick{
	unsigned int x_value;
	unsigned int y_value;
	unsigned int x_pin;
	unsigned int y_pin;
} Joystick;

void joystick_init(Joystick* joystick, unsigned int x_pin, unsigned int y_pin);

void read_joystick_task(void* joystick);

#endif /* JOYSTICK_H_ */