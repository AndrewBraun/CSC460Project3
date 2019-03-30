 /*
 * joystick.c
 *
 * Created: 2019-03-18 3:31:37 PM
 *  Author: Andrew
 */
#include <avr/io.h>
#include "joystick.h"

// ADCSRA

// ADC credit: https://maker.pro/custom/tutorial/how-to-take-analog-readings-with-an-avr-microcontroller
// https://www.avrfreaks.net/forum/how-properly-read-analog-pin
void joystick_init(Joystick* joystick, uint8_t x_pin, uint8_t y_pin){
	DDRF = 0x00;
	
	ADMUX |= (1 << REFS0); // Sets reference to VCC
	ADMUX |= (1 << ADLAR); // Left justifies the output, so that we can read value into 8-bit int with minimal loss.
	ADCSRA |= (1 << ADEN); // Enable ADC
		
	joystick->x_pin = x_pin;
	joystick->y_pin = y_pin;
	joystick->x_value = 128;
	joystick->y_value = 128;
}

void read_joystick_task(void* joystick_arg){
	Joystick* joystick = (Joystick *) joystick_arg;
	
	ADMUX = (1 << REFS0) | (1 << ADLAR) | joystick->x_pin; // Sets ADC to look at x pin.
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	
	// We throw away the first reading to prevent a glitch in which
	// the reading from the other joystick is used for this one.
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	
	joystick->x_value = (1 - JOYSTICK_ALPHA) * ADCH + JOYSTICK_ALPHA * joystick->x_value;
	
	ADMUX = (1 << REFS0) | (1 << ADLAR) | joystick->y_pin; // Sets ADC to look at y pin.
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	
	// We throw away the first reading to prevent a glitch in which
	// the reading from the other joystick is used for this one.
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete

	joystick->y_value = (1 - JOYSTICK_ALPHA) * ADCH + JOYSTICK_ALPHA * joystick->y_value;
}