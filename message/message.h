/**
 * Utilities for encoding and decoding messages from the controller.
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

// Possible commands which can be sent. Opcodes are one byte, so there
// should be a maximum of 256 possible operations.
typedef enum {
    Cmd_MoveRoomba
} CmdOpCode_t;

/*
 * Command: Move Roomba
 */

typedef struct {
    int16_t velocity;
    int16_t radius;
} CmdMoveRoombaArgs_t;

int8_t CmdMoveRoomba_encode(char **msgBuf, CmdMoveRoombaArgs_t const* args);
CmdMoveRoombaArgs_t *CmdMoveRoomba_decode(char const* msgBuf);

/**
 * Generic functions for encoding and decoding
 */

/**
 * Decodes the next incoming message from Bluetooth (UART 1).
 * If the message was successfully decoded, dispatches a sporadic message handler task.
 */
int8_t Cmd_decodenext();

/**
 * Frees heap-allocated memory for a command arguments structure.
 */
void CmdArgs_free(void* args);

/**
 * Frees heap-allocated memory for a message buffer structure.
 */
void CmdMsgBuf_free(char* msgBuf);

/**
 * Message handler functions
 */
typedef struct {
    void (*HandleCmd_MoveRoomba)(CmdMoveRoombaArgs_t*);     // Move Roomba handler
} MessageHandlerVect;

extern MessageHandlerVect g_messageHandlers;

#endif
