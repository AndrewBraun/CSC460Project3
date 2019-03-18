/*
 * button.h
 *
 * Created: 2019-03-18 3:47:09 PM
 *  Author: Andrew
 */ 
#ifndef BUTTON_H_
#define BUTTON_H_

#define BUTTON_PORT	0x80

// Current value of laser according to controller.
// Actual state will differ if time is up.
int laser_on = 0;

// Previous value of the button
int button_value = 1;

void button_init();

void read_laser_button_task(void* param_ptr);

#endif /* BUTTON_H_ */