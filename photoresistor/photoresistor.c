/*
 * photoresistor.c
 *
 * Created: 2019-04-01 3:20:45 PM
 *  Author: Andrew
 */
#include "../roomba/roomba.h"

#define PHOTORESISTOR_PIN PK8

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
	uint8_t death_song[12] = {103, 110, 0, 110, 110, 108, 107, 103, 100, 0, 100, 96};
	uint8_t song_timings[12] = {16, 16, 16, 16, 16, 16, 32, 16, 16, 16, 16, 16};
	Roomba_LoadSong(0, death_song, song_timings, 12);
	Roomba_PlaySong(0);
	
	while(1);
}

void read_photoresistor_task(void* param_ptr){
	ADMUX = (1 << REFS0) | (1 << ADLAR) | PHOTORESISTOR_PIN; // Sets ADC to look at photoresistor.
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	
	// We throw away the first reading to prevent reading the wrong value
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	
	// If the robot is not being pointed at, set health to 20.
	if (ADCH >= 300) {
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