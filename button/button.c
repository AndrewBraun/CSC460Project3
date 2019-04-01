/*
 * button.c
 *
 * Created: 2019-03-18 3:50:58 PM
 *  Author: Andrew
 */ 
#include <avr/io.h>
#include "button.h"
#include "../uart/uart.h"
#include "../message/message.h"

// The port that we read the button from
#define BUTTON_PORT	PH5

// Current value of laser according to controller.
uint8_t laser_on = 0;

// Previous value of the button
uint8_t button_value = 1;

// Time left for the laser.
// Value start at 100 (10.0 seconds)
// Decremented by 1 every time laser is on (every 100 ms).
int8_t laser_time = 100;

// Pull-up resistor credit: https://www.avrfreaks.net/forum/pull-resistor
void button_init(){
	DDRH &= ~(1 << BUTTON_PORT); // Set button port to input
	PORTH |= (1 << BUTTON_PORT); // Activate pullup resistor
}

void turnOnLaser(){
	laser_on = 1;
	uart_putchar(UART_1, Cmd_TurnOnLaser);
}

void turnOffLaser(){
	laser_on = 0;
	uart_putchar(UART_1, Cmd_TurnOffLaser);
}

// Button not pushed = 0x20
// Button pushed = 0x00
void read_laser_button_task(void* param_ptr){
	
	//Skip reading the button if we don't have any more laser time.
	if (laser_time <= 0) return;
	
	// Decrement the laser time if the laser is on.
	// If the laser time is up, turn off the laser.
	if (laser_on){
		laser_time--;
		if (laser_time <= 0) {
			turnOffLaser();
			return;
		}
	}
	
	// Read button value
	// Credit: http://forum.arduino.cc/index.php?topic=91079.15
	uint8_t new_button_value = (PINH & (1 << BUTTON_PORT));
	
	// If the button is being pushed down and the previous value is not "pushed"
	if (!new_button_value && button_value) {

		if (!laser_on && laser_time){
			turnOnLaser();
		}
		else {
			turnOffLaser();
		}
	}
	
	button_value = new_button_value;
}