/*
 * button.h
 *
 * Created: 2019-03-18 3:47:09 PM
 *  Author: Andrew
 */ 
#ifndef BUTTON_H_
#define BUTTON_H_

/*
 * Only the controller needs to call this function.
 */
void button_init();

/*
 * Read the button's current value
 * Will send a message to turn off the laser if either conditino is met:	
 *		a) The person is holding down the button and they weren't pushing it down already. The laser is on.
 *		b) The laser runs out of time
 * Will send a message to turn on the laser if all conditions are met:
 *		a) The person is holding down the button and they weren't pushing it down already
 *		b) The laser is currently off
 *		c) The laser still has time left
 */
void read_laser_button_task(void* param_ptr);

#endif /* BUTTON_H_ */