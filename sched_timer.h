/**
 * Timer routines for TTA scheduler.
 */

#include <stdint.h>

#ifndef SCHEDTIMER_H
#define SCHEDTIMER_H

/**
 * Initializes the scheduler timer.
 */
void Schedtimer_Init();

/**
 * @return The number of milliseconds since Schedtimer_init() was called.
 */
int16_t millis();

#endif
