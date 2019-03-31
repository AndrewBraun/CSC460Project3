/**
 * main_roomba.c
 * Roomba program entry point.
 */

#include "uart/uart.h"
#include "message/message.h"
#include "roomba/roomba.h"
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
	uart_init(UART_0, UART_9600);
	CmdMoveRoombaArgs_t inArgs = { .velocity = 1, .radius = 1 };

	char *msg;
	int len = CmdMoveRoomba_encode(&msg, &inArgs);
	CmdMoveRoombaArgs_t *args = CmdMoveRoomba_decode(msg);

	CmdArgs_free(args);

	while(1);
}
#endif

struct {
	int16_t velocity;
	int16_t radius;
} g_lastControllerArgs;

roomba_sensor_data_t g_lastRoombaSensorData;

void HandleCmd_MoveRoomba(CmdMoveRoombaArgs_t* args)
{
	g_lastControllerArgs.velocity = args->velocity;
	g_lastControllerArgs.radius = args->radius;

	// Command handlers are responsible for freeing args!
	CmdArgs_free(args);
}

void Task_PollBluetooth(void* args)
{
	DDRB = 0xFF;

	// Bluetooth should be on UART 1
	if (uart_bytes_received(UART_1) > 0)
	{
		PORTB = 0xFF;

		// Decode next command from UART 1. This may dispatch a sporadic task
		// to handle the new message.
		Cmd_decodenext();

		PORTB = 0x00;
	}

}

void Task_PollRoombaSensors(void* args)
{
	Roomba_UpdateSensorPacket(EXTERNAL, &g_lastRoombaSensorData);
}

void Task_UpdateRoombaSpeed(void* args)
{
	DDRB = 0xFF;
	PORTB = 0xFF;

	// In the interest of not smashing my computer, default to straight for now.
	Roomba_Drive(g_lastControllerArgs.velocity, 0x8000);
	// TODO: add check for last bluetooth update time?
	//    If we haven't gotten a command from the joystick in ~2-3 seconds, stop.

	PORTB = 0x00;
}

int main() 
{
	Roomba_Init();
	uart_init(UART_1, UART_9600);

	g_messageHandlers.HandleCmd_MoveRoomba = HandleCmd_MoveRoomba;

	Scheduler_Init();
	Scheduler_StartPeriodicTask(0,  100, Task_PollBluetooth,     NULL);
	Scheduler_StartPeriodicTask(25, 100, Task_UpdateRoombaSpeed, NULL);
	Scheduler_StartPeriodicTask(50, 500, Task_PollRoombaSensors, NULL);
	Scheduler_Start();
}
