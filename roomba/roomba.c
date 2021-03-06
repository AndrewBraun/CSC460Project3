/*
 * roomba.c
 *
 *  Created on: 4-Feb-2009
 *      Author: nrqm
 */

#include <stdlib.h>
#include <util/delay.h>

#include "../uart/uart.h"
#include "roomba.h"
#include "roomba_sci.h"
#include "../tta.h"
//#include "sensor_struct.h"

#define LOW_BYTE(v)   ((unsigned char) (v))
#define HIGH_BYTE(v)  ((unsigned char) (((unsigned int) (v)) >> 8))

// Connect DD port to digital port 2
#define DD_DDR DDRE
#define DD_PORT PORTE
#define DD_PIN PE4

#define ROOMBA_UART UART_2

STATUS_LED_STATE status = LED_OFF;
LED_STATE spot = LED_OFF;
LED_STATE clean = LED_OFF;
LED_STATE max = LED_OFF;
LED_STATE dd = LED_OFF;
uint8_t power_colour = 0;		// green
uint8_t power_intensity = 255;	// full intensity

ROOMBA_STATE state = SAFE_MODE;

static void update_leds();

void Roomba_Init()
{
	uint8_t i;
	DD_DDR |= _BV(DD_PIN);
	// Wake up the Roomba by driving the DD pin low for 500 ms.
	DD_PORT &= ~_BV(DD_PIN);
	_delay_ms(500);
	DD_PORT |= _BV(DD_PIN);

	// Wait for 2 seconds, Then pulse the DD pin 3 times to set the Roomba to operate at 19200 baud.
	// This ensures that we know what baud rate to talk at.
	_delay_ms(2000);
	for (i = 0; i < 3; i++)
	{
		DD_PORT &= ~_BV(DD_PIN);
		_delay_ms(50);
		DD_PORT |= _BV(DD_PIN);
		_delay_ms(50);
	}

	uart_init(ROOMBA_UART, UART_19200);

	// start the Roomba's SCI
	uart_putchar(ROOMBA_UART, START);
	_delay_ms(20);

	// put the Roomba into safe mode.
	uart_putchar(ROOMBA_UART, CONTROL);
	_delay_ms(20);

	// Set the Roomba's LEDs to the defaults defined above (to verify defaults).
	//update_leds();
}

/**
 * Use this function instead of the while loops in Roomba_UpdateSensorPacket if you have a system
 * clock.  This will add a timeout when it's waiting for the bytes to come in, so that the
 * function doesn't enter an infinite loop if a byte is missed.  You'll have to modify this function
 * and insert it into Roomba_UpdateSensorPacket to suit your application.
 */
/*
uint8_t wait_for_bytes(uint8_t num_bytes, uint8_t timeout)
{
	uint16_t start;
	start = Now();	// current system time
	while (Now() - start < timeout && uart_bytes_received(ROOMBA_UART) != num_bytes);
	if (uart_bytes_received(ROOMBA_UART) >= num_bytes)
		return 1;
	else
		return 0;
}*/

static void Roomba_ReadSensorParamsUpdate(void* args)
{
	
	ROOMBA_SENSOR_GROUP group = ((RoombaSensorUpdateArgs_t*)args)->group;
	roomba_sensor_data_t* sensor_packet = ((RoombaSensorUpdateArgs_t*)args)->sensor_packet;

	switch(group)
	{
	case EXTERNAL:
		// environment sensors
		PORTB = 0xFF;
		if (uart_bytes_received(ROOMBA_UART) < 10)  // skip update if we haven't gotten all bytes
			break;
		uart_putchar(UART_0, uart_bytes_received(ROOMBA_UART));
		
		sensor_packet->bumps_wheeldrops = uart_get_byte(ROOMBA_UART, 0);
		sensor_packet->wall = uart_get_byte(ROOMBA_UART, 1);
		sensor_packet->cliff_left = uart_get_byte(ROOMBA_UART, 2);
		sensor_packet->cliff_front_left = uart_get_byte(ROOMBA_UART, 3);
		sensor_packet->cliff_front_right = uart_get_byte(ROOMBA_UART, 4);
		sensor_packet->cliff_right = uart_get_byte(ROOMBA_UART, 5);
		sensor_packet->virtual_wall = uart_get_byte(ROOMBA_UART, 6);
		sensor_packet->motor_overcurrents = uart_get_byte(ROOMBA_UART, 7);
		sensor_packet->dirt_left = uart_get_byte(ROOMBA_UART, 8);
		sensor_packet->dirt_right = uart_get_byte(ROOMBA_UART, 9);

		uart_putchar(UART_0, sensor_packet->virtual_wall);
		break;
	case CHASSIS:
		// chassis sensors
		if (uart_bytes_received(ROOMBA_UART) < 6)
			break;
		sensor_packet->remote_opcode = uart_get_byte(ROOMBA_UART, 0);
		sensor_packet->buttons = uart_get_byte(ROOMBA_UART, 1);
		sensor_packet->distance.bytes.high_byte = uart_get_byte(ROOMBA_UART, 2);
		sensor_packet->distance.bytes.low_byte = uart_get_byte(ROOMBA_UART, 3);
		sensor_packet->angle.bytes.high_byte = uart_get_byte(ROOMBA_UART, 4);
		sensor_packet->angle.bytes.low_byte = uart_get_byte(ROOMBA_UART, 5);
		break;
	case INTERNAL:
		// internal sensors
		if (uart_bytes_received(ROOMBA_UART) < 10)
			break;
		sensor_packet->charging_state = uart_get_byte(ROOMBA_UART, 0);
		sensor_packet->voltage.bytes.high_byte = uart_get_byte(ROOMBA_UART, 1);
		sensor_packet->voltage.bytes.low_byte = uart_get_byte(ROOMBA_UART, 2);
		sensor_packet->current.bytes.high_byte = uart_get_byte(ROOMBA_UART, 3);
		sensor_packet->current.bytes.low_byte = uart_get_byte(ROOMBA_UART, 4);
		sensor_packet->temperature = uart_get_byte(ROOMBA_UART, 5);
		sensor_packet->charge.bytes.high_byte = uart_get_byte(ROOMBA_UART, 6);
		sensor_packet->charge.bytes.low_byte = uart_get_byte(ROOMBA_UART, 7);
		sensor_packet->capacity.bytes.high_byte = uart_get_byte(ROOMBA_UART, 8);
		sensor_packet->capacity.bytes.low_byte = uart_get_byte(ROOMBA_UART, 9);
		break;
	}
	uart_reset_receive(ROOMBA_UART);
	//free(args);
	PORTB = 0x00;
}

void Roomba_AsyncUpdateSensorPacket(ROOMBA_SENSOR_GROUP group, roomba_sensor_data_t* sensor_packet)
{
	uart_putchar(ROOMBA_UART, SENSORS);
	uart_putchar(ROOMBA_UART, group);
	
	RoombaSensorUpdateArgs_t* args = malloc(sizeof(RoombaSensorUpdateArgs_t));
	args->group = group;
	args->sensor_packet = sensor_packet;

	Scheduler_AddSporadicTask(20, 3, Roomba_ReadSensorParamsUpdate, args);
}

void Roomba_ChangeState(ROOMBA_STATE newState)
{
	if (newState == SAFE_MODE)
	{
		if (state == PASSIVE_MODE)
			uart_putchar(ROOMBA_UART, CONTROL);
		else if (state == FULL_MODE)
			uart_putchar(ROOMBA_UART, SAFE);
	}
	else if (newState == FULL_MODE)
	{
		Roomba_ChangeState(SAFE_MODE);
		uart_putchar(ROOMBA_UART, FULL);
	}
	else if (newState == PASSIVE_MODE)
	{
		uart_putchar(ROOMBA_UART, POWER);
	}
	else
	{
		// already in the requested state
		return;
	}

	state = newState;
	_delay_ms(20);
}

void Roomba_Drive( int16_t velocity, int16_t radius )
{
	uart_putchar(ROOMBA_UART, DRIVE);
	uart_putchar(ROOMBA_UART, HIGH_BYTE(velocity));
	uart_putchar(ROOMBA_UART, LOW_BYTE(velocity));
	uart_putchar(ROOMBA_UART, HIGH_BYTE(radius));
	uart_putchar(ROOMBA_UART, LOW_BYTE(radius));
}

void Roomba_Drive_Direct(int16_t right, int16_t left) {
	uart_putchar(ROOMBA_UART, D_DIRECT);
	uart_putchar(ROOMBA_UART, HIGH_BYTE(right));
	uart_putchar(ROOMBA_UART, LOW_BYTE(right));
	uart_putchar(ROOMBA_UART, HIGH_BYTE(left));
	uart_putchar(ROOMBA_UART, LOW_BYTE(left));
}

/**
 * Update the LEDs on the Roomba to match the configured state
 */
void update_leds()
{
	// The status, spot, clean, max, and dirt detect LED states are combined in a single byte.
	uint8_t leds = status << 4 | spot << 3 | clean << 2 | max << 1 | dd;

	uart_putchar(ROOMBA_UART, LEDS);
	uart_putchar(ROOMBA_UART, leds);
	uart_putchar(ROOMBA_UART, power_colour);
	uart_putchar(ROOMBA_UART, power_intensity);
}

void Roomba_ConfigPowerLED(uint8_t colour, uint8_t intensity)
{
	power_colour = colour;
	power_intensity = intensity;
	update_leds();
}

void Roomba_ConfigStatusLED(STATUS_LED_STATE state)
{
	status = state;
	update_leds();
}

void Roomba_ConfigSpotLED(LED_STATE state)
{
	spot = state;
	update_leds();
}

void Roomba_ConfigCleanLED(LED_STATE state)
{
	clean = state;
	update_leds();
}

void Roomba_ConfigMaxLED(LED_STATE state)
{
	max = state;
	update_leds();
}

void Roomba_ConfigDirtDetectLED(LED_STATE state)
{
	dd = state;
	update_leds();
}

void Roomba_LoadSong(uint8_t songNum, uint8_t* notes, uint8_t* notelengths, uint8_t numNotes)
{
	uint8_t i = 0;

	uart_putchar(ROOMBA_UART, SONG);
	uart_putchar(ROOMBA_UART, songNum);
	uart_putchar(ROOMBA_UART, numNotes);

	for (i=0; i<numNotes; i++)
	{
		uart_putchar(ROOMBA_UART, notes[i]);
		uart_putchar(ROOMBA_UART, notelengths[i]);
	}
}

void Roomba_PlaySong(int songNum)
{
	uart_putchar(ROOMBA_UART, PLAY);
	uart_putchar(ROOMBA_UART, songNum);
}

uint8_t Roomba_BumperActivated(roomba_sensor_data_t* sensor_data)
{
	// if either of the bumper bits is set, then return true.
	return (sensor_data->bumps_wheeldrops & 0x03) != 0;
}

