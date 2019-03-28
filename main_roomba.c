/**
 * main_roomba.c
 * Roomba program entry point.
 */

#include "uart/uart.h"
#include "message/message.h"
#include "tta.h"

#include <stddef.h>

#if 0
void Test_MessageEncode()
{
	uart_init(UART_19200);

	CmdMoveRoombaArgs_t args;
	args.wheelLeft = 127;
	args.wheelRight = 127;

	char* msgBuf;
	int len = CmdMoveRoomba_encode(&msgBuf, &args);

	// Turn on a light if we get an error
	if (len == -1)
	{
		DDRB = 0xFF;
		PORTB = 0xFF;
	}

	for (int i = 0; i < len; i++)
		uart_putchar(msgBuf[i]);

	while (1);
}

void Test_MessageDecode()
{
	uart_init(UART_9600);
	CmdMoveRoombaArgs_t inArgs = { .wheelLeft = 127, .wheelRight = -127 };

	char *msg;
	int len = CmdMoveRoomba_encode(&msg, &inArgs);

	uart_putchar(inArgs.wheelLeft);
	uart_putchar(inArgs.wheelRight);

	CmdMoveRoombaArgs_t *args = CmdMoveRoomba_decode(msg);
	uart_putchar(args->wheelLeft);
	uart_putchar(args->wheelRight);

	CmdArgs_free(args);

	while(1);
}
#endif

void Task_PollBluetooth(void* args)
{
	DDRB = 0xFF;
	PORTB = 0xFF;

	// Bluetooth should be on UART 1
	if (uart_bytes_received_1() > 0)
	{

		for (int i = 0; i < uart_bytes_received_1(); i++)
			uart_putchar_0(uart_get_byte_1(i));

		// Clear UART buffer
		uart_reset_receive_1();
	}

	PORTB = 0x00;
}

int main() 
{
	uart_init(UART_9600);

	Scheduler_Init();
	Scheduler_StartPeriodicTask(0, 100, Task_PollBluetooth, NULL);
	Scheduler_Start();
}
