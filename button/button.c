/*
 * button.c
 *
 * Created: 2019-03-18 3:50:58 PM
 *  Author: Andrew
 */ 
#include <avr/io.h>
#include "button.h"

void button_init(){
	DDRB &= ~(BUTTON_PORT); // Set button port to input
}

void read_laser_button_task(void* param_ptr){
	/*
	Read button value
	int new_button_value = PORTB
	
	if (!new_button_value && button_value) {
		laser_on = (laser_on + 1) % 2;
	}
	
	button_value = new_button_value
	*/
}