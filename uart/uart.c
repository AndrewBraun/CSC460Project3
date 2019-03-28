/**
 * @file   uart.c
 * @author Justin Tanner
 * @date   Sat Nov 22 21:32:03 2008
 *
 * @brief  UART Driver targetted for the AT90USB1287
 *
 */
#include "uart.h"

#define F_CPU 8000000UL

#ifndef F_CPU
#warning "F_CPU not defined for uart.c."
#define F_CPU 11059200UL
#endif

static volatile uint8_t uart_buffer[UART_BUFFER_SIZE];
static volatile uint8_t uart_buffer_index;

/**
 * Initalize UART
 *
 */
void uart_init(UART_BPS bitrate)
{
	//UCSR1A = (1 << U2X1);
	UCSR1B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	UCSR1C = (1 << UCSZ01) | (1 << UCSZ00);

	UBRR0H = 0;	// for any speed >= 9600 bps, the UBBR value fits in the low byte.
	UBRR1H = 0;

	// See the appropriate AVR hardware specification for a table of UBBR values at different
	// clock speeds.
	switch (bitrate)
	{
#if F_CPU==8000000UL
	case UART_9600:
		UBRR0L = 104;
		UBRR1L = 208;
	case UART_19200:
		UBRR0L = 51;
		UBRR1L = 102;
		break;
	case UART_38400:
		UBRR0L = 25;
		UBRR1L = 50;
		break;
	case UART_57600:
		UBRR0L = 16;
		UBRR1L = 32;
		break;
	default:
		UBRR0L = 51;
		UBRR1L = 102;
#elif F_CPU==16000000UL
	case UART_9600:
		UBRR0L = 208;
		UBRR1L = 416;
	case UART_19200:
		UBRR0L = 103;
		UBRR1L = 206;
		break;
	case UART_38400:
		UBRR0L = 51;
		UBRR1L = 102;
		break;
	case UART_57600:
		UBRR0L = 34;
		UBRR1L = 68;
		break;
	default:
		UBRR0L = 103;
		UBRR1L = 206;
#elif F_CPU==18432000UL
	case UART_19200:
		UBRR0L = 119;
		UBRR1L = 238;
		break;
	case UART_38400:
		UBRR0L = 59;
		UBRR1L = 118;
		break;
	case UART_57600:
		UBRR0L = 39;
		UBRR1L = 78;
		break;
	default:
		UBRR0L = 119;
		UBRR1L = 238;
		break;
#else
#warning "F_CPU undefined or not supported in uart.c."
	default:
		UBRR0L = 71;
		UBRR1L = 142;
		break;
#endif
	}

    uart_buffer_index = 0;
}

/**
 * Transmit one byte
 * NOTE: This function uses busy waiting
 *
 * @param byte data to trasmit
 */
void uart_putchar_0(uint8_t byte)
{
    /* wait for empty transmit buffer */
    while (!( UCSR0A & (1 << UDRE0)));

    /* Put data into buffer, sends the data */
    UDR0 = byte;
}

void uart_putchar_1(uint8_t byte){
	/* wait for empty transmit buffer */
	while (!( UCSR1A & (1 << UDRE1)));

	/* Put data into buffer, sends the data */
	UDR1 = byte;
}

/**
 * Receive a single byte from the receive buffer
 *
 * @param index
 *
 * @return
 */
uint8_t uart_get_byte(int index)
{
    if (index < UART_BUFFER_SIZE)
    {
        return uart_buffer[index];
    }
    return 0;
}

/**
 * Get the number of bytes received on UART
 *
 * @return number of bytes received on UART
 */
uint8_t uart_bytes_received(void)
{
    return uart_buffer_index;
}

/**
 * Prepares UART to receive another payload
 *
 */
void uart_reset_receive(void)
{
    uart_buffer_index = 0;
}

/**
 * UART receive byte ISR
 */
ISR(USART0_RX_vect)
{
	while(!(UCSR0A & (1<<RXC0)));
    uart_buffer[uart_buffer_index] = UDR0;
    uart_buffer_index = (uart_buffer_index + 1) % UART_BUFFER_SIZE;
}
