/**
 * Timer routines for TTA scheduler.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "sched_timer.h"

#define Disable_Interrupt()		asm volatile ("cli"::)
#define Enable_Interrupt()		asm volatile ("sei"::)

static volatile int16_t g_timerMs = 0;  // milliseconds
static volatile int16_t g_timerMsFractional = 0;  // thousandths of a millisecond

/**
 * Initializes the scheduler timer.
 */
void Schedtimer_Init() 
{
    TCCR1B |= ((1<<CS12)|(1<<CS10));  // Prescale: (clock / 1024)

    OCR1A = 16;  // 1.024 msec = 16 ticks
    TIMSK1 |= (1 << OCIE1A);  // Enable timer 1 compare interrupt

    // Init timer debug pin (PA0/Digital pin 22)
    DDRA |= (1 << PA0);
    PORTA &= ~(1 << PA0);
}

/**
 * @return The number of milliseconds since Schedtimer_init() was called.
 */
int16_t millis()
{
    return g_timerMs;
}

SIGNAL(TIMER1_COMPA_vect)
{
    Disable_Interrupt();
    PORTA |= (1 << PA0);

    // Our prescaler uses (clock / 1024), which at 16Mhz gives 15625 cycles/sec.
    // At this rate, 16 ticks gives us 1.024 ms, so we'll track both the whole
    // part (1 ms every 16 ticks) and the fractional part (0.024ms every 16 ticks)
    // to get an accurate time.
    uint16_t ticks = TCNT1 / 16;
    g_timerMs += ticks;  // whole part (1ms)

    // Calculate fractional part
    g_timerMsFractional = g_timerMsFractional + 24 * ticks;
    g_timerMs += g_timerMsFractional / 1000;
    g_timerMsFractional = g_timerMsFractional % 1000;

    // Reset TCNT1
    TCNT1 = TCNT1 % 16;

    PORTA &= ~(1 << PA0);
    Enable_Interrupt();
}
