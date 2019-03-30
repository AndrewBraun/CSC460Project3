/**
 * @file   uart.c
 * @author Justin Tanner
 * @date   Sat Nov 22 21:32:03 2008
 *
 * @brief  UART Driver targetted for the AT90USB1287
 *
 */
#include "uart.h"

#define F_CPU 16000000UL

// UART receive buffers
static volatile uint8_t uart_buffer_0[UART_BUFFER_SIZE];
static volatile uint8_t uart_buffer_1[UART_BUFFER_SIZE];
static volatile uint8_t uart_buffer_index_0;
static volatile uint8_t uart_buffer_index_1;

/**
 * Initalize UART
 *
 */
void uart_init(uint8_t uart_id, UART_BPS bitrate){
	uint8_t brLow;

	// See the appropriate AVR hardware specification for a table of UBBR values at different
	// clock speeds.
	switch (bitrate)
	{
#if F_CPU==8000000UL
	case UART_9600:
		brLow = 104;
		break;
	case UART_19200:
		brLow = 51;
		break;
	case UART_38400:
		brLow = 25;
		break;
	case UART_57600:
		brLow = 16;
		break;
	default:
		brLow = 51;
#elif F_CPU==16000000UL
	case UART_9600:
		brLow = 208;
		break;
	case UART_19200:
		brLow = 103;
		break;
	case UART_38400:
		brLow = 51;
		break;
	case UART_57600:
		brLow = 34;
		break;
	default:
		brLow = 103;
#elif F_CPU==18432000UL
	case UART_19200:
		brLow = 119;
		break;
	case UART_38400:
		brLow = 59;
		break;
	case UART_57600:
		brLow = 39;
		break;
	default:
		brLow = 119;
		break;
#else
#warning "F_CPU undefined or not supported in uart.c."
	default:
		brLow = 71;
		break;
#endif
	}

	switch (uart_id)
	{
	case UART_0:
		// Enable Tx/Rx, set data frame options
		UCSR0A = (1 << U2X0);
		UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
		UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

		// Set registers according to calculated baud rate
		UBRR0L = brLow;  // for any speed >= 9600 bps, the UBBR value fits in the low byte.
		UBRR0H = 0;
		break;
	case UART_1:
		UCSR1A = (1 << U2X1);
		UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
		UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);

		UBRR1L = brLow;
		UBRR1H = 0;
		break;
	}

    uart_buffer_index_0 = 0;
	uart_buffer_index_1 = 0;
}

/**
 * Transmit one byte
 * NOTE: This function uses busy waiting
 *
 * @param byte data to trasmit
 */
void uart_putchar(uint8_t uart_id, uint8_t byte)
{
	switch (uart_id)
	{
	case UART_0:
		/* wait for empty transmit buffer */
		while (!( UCSR0A & (1 << UDRE0)));

		/* Put data into buffer, sends the data */
		UDR0 = byte;
		break;
	case UART_1:
		/* wait for empty transmit buffer */
		while (!( UCSR1A & (1 << UDRE1)));

		/* Put data into buffer, sends the data */
		UDR1 = byte;
		break;
	}
}

/**
 * Receive a single byte from the receive buffer
 *
 * @param index
 *
 * @return
 */
uint8_t uart_get_byte(uint8_t uart_id, int index)
{
	switch (uart_id)
	{
	case UART_0:
		if (index < UART_BUFFER_SIZE)
		{
			return uart_buffer_0[index];
		}
		return 0;
	case UART_1:
		if (index < UART_BUFFER_SIZE){
			return uart_buffer_1[index];
		}
		return 0;
	}
}

/**
 * Get the number of bytes received on UART
 *
 * @return number of bytes received on UART
 */
uint8_t uart_bytes_received(uint8_t uart_id)
{
	switch(uart_id)
	{
	case UART_0:
		return uart_buffer_index_0;
	case UART_1:
		return uart_buffer_index_1;	
	}
}

/**
 * Prepares UART to receive another payload
 */
void uart_reset_receive(uint8_t uart_id)
{
	switch (uart_id)
	{
	case UART_0:
		uart_buffer_index_0 = 0;
	case UART_1:
		uart_buffer_index_1 = 0;
	}
}

/**
 * UART receive byte ISR
 */
ISR(USART0_RX_vect)
{
	while(!(UCSR0A & (1<<RXC0)));
    uart_buffer_0[uart_buffer_index_0] = UDR0;
    uart_buffer_index_0 = (uart_buffer_index_0 + 1) % UART_BUFFER_SIZE;
}

ISR(USART1_RX_vect){
	while(!(UCSR1A & (1<<RXC1)));
    uart_buffer_1[uart_buffer_index_1] = UDR1;
    uart_buffer_index_1 = (uart_buffer_index_1 + 1) % UART_BUFFER_SIZE;
}
