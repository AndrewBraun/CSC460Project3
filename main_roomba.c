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
	int16_t right;
	int16_t left;
} g_lastControllerArgs;

roomba_sensor_data_t g_lastRoombaSensorData;

void HandleCmd_MoveRoomba(CmdMoveRoombaArgs_t* args)
{
	g_lastControllerArgs.right = args->right;
	g_lastControllerArgs.left = args->left;

	// Command handlers are responsible for freeing args!
	CmdArgs_free(args);
}

void Task_PollBluetooth(void* args)
{

	// Bluetooth should be on UART 1
	if (uart_bytes_received(UART_1) > 0)
	{
		// Decode next command from UART 1. This may dispatch a sporadic task
		// to handle the new message.
		Cmd_decodenext();

	}
}

/*
 * The amount of times (run-throughs of Task_UpdateRoombaSpeed)
 * left where the user does not have control.
 * Will go into this state if a bumper/IR wall is detected.
 */
uint8_t autonomous_time = 0;

void Task_PollRoombaSensors(void* args)
{
	Roomba_AsyncUpdateSensorPacket(EXTERNAL, &g_lastRoombaSensorData);
	if (g_lastRoombaSensorData.bumps_wheeldrops & 0x0F || g_lastRoombaSensorData.virtual_wall) {
		// Go into autonomous mode
		Roomba_Drive_Direct(-400, -400);
		autonomous_time = 10;
	}
}

void Task_UpdateRoombaSpeed(void* args){
	// If the Roomba is in autonomous mode
	if (autonomous_time) {
		autonomous_time--;
	}
	// If the Roomba is in user-control mode
	else {
			Roomba_Drive_Direct(g_lastControllerArgs.right, g_lastControllerArgs.left);
	// TODO: add check for last bluetooth update time?
	//    If we haven't gotten a command from the joystick in ~2-3 seconds, stop.
	}
}

int main() 
{
	DDRB = 0xff;
	Roomba_Init();
	uart_init(UART_1, UART_9600);

	g_messageHandlers.HandleCmd_MoveRoomba = HandleCmd_MoveRoomba;

	Scheduler_Init();
	Scheduler_StartPeriodicTask(0,  100, Task_PollBluetooth,     NULL);
	Scheduler_StartPeriodicTask(25, 100, Task_UpdateRoombaSpeed, NULL);
	Scheduler_StartPeriodicTask(50, 500, Task_PollRoombaSensors, NULL);
	Scheduler_Start();
}
