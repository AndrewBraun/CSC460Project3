/*
 * photoresistor.c
 *
 * Created: 2019-04-01 3:20:45 PM
 *  Author: Andrew
 */
#include <avr/io.h>
#include <util/delay.h>
#include "../roomba/roomba.h"
#include "../uart/uart.h"

#define PHOTORESISTOR_PIN PF0

#define PHOTORESISTOR_THRESH 45

/*
 * The "health" of the robot.
 * It starts at 20 (20 0.1 second pieces)
 * For every continuous 0.1 second period the laser is pointed at the robot,
 * the health is decremented.
 * If the light source stops pointing at the laser,
 * the health goes back to 20.
 * If the health goes to 0, it goes to the shadow realm,
 * which the robot cannot escape from.
*/
int8_t health = 20;

/*
 * Inescapable state where the robot is dead.
 * Stops robot and plays death song.
 */
void shadow_realm() {
	

	// Write "stop moving" to Roomba
	Roomba_Drive(0, 0);
	
	// Play death song
	// Credit: http://www.mariopiano.com/mario-sheet-music-death-sound.html
	uint8_t death_song[12] = {79, 86, 0, 86, 86, 84, 83, 79, 76, 0, 76, 72};
	uint8_t song_timings[12] = {16, 16, 16, 16, 16, 16, 32, 16, 16, 16, 16, 16};
	Roomba_LoadSong(0, death_song, song_timings, 12);
	Roomba_PlaySong(0);
	
	while(1);
}

void photoresistor_init(){
	DDRF &= ~(1 << PHOTORESISTOR_PIN);
	ADCSRA |= (1 << ADEN);
}

void Task_UpdatePhotoresistor(void* param_ptr){
	ADMUX = (1 << REFS0) | (1 << ADLAR); // Sets ADC to look at photoresistor.
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	
	// We throw away the first reading to prevent reading the wrong value
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	
	// If the robot is not being pointed at, set health to 20.
	//uint16_t nextVal = (ADCL | (ADCH << 8));

	uart_init(UART_0, UART_9600);
	//uart_putchar(UART_0, ADCL);
	uart_putchar(UART_0, ADCH);

	if (ADCH <= PHOTORESISTOR_THRESH) {
		health = 20;
	}
	// If the robot is being pointed at
	else {
		health--;
		if (!health){
			shadow_realm();
		}
	}
}