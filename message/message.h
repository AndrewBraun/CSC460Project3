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
    int16_t wheelLeft;
    int16_t wheelRight;
} CmdMoveRoombaArgs_t;

int8_t CmdMoveRoomba_encode(char **msgBuf, CmdMoveRoombaArgs_t const* args);

/**
 * Generic functions for encoding and decoding
 */

/**
 * Decodes a message from a string buffer.
 * @param msgBuf: Incoming message buffer
 * @param opcode [out]: Opcode of the incoming message
 * @param args [out]: Any arguments associated with the incoming command
 */
int8_t Cmd_decode(char const* msgBuf, uint8_t *opcode /*out*/, void **args /*out*/);

/**
 * Frees heap-allocated memory for a command arguments structure.
 */
int8_t CmdArgs_free(void* args);

#endif
