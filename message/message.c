/**
 * Utilities for encoding and decoding messages from the controller.
 */

#include <stdlib.h>
#include "../uart/uart.h"

#include "message.h"

int8_t CmdMoveRoomba_encode(char **msgBuf /*out*/, CmdMoveRoombaArgs_t const* args)
{
    *msgBuf = malloc(sizeof(CmdMoveRoombaArgs_t) + 1);  // arguments + opcode

    (*msgBuf)[0] = Cmd_MoveRoomba;                      // Byte 0: opcode
    (*msgBuf)[1] = (uint8_t) (args->wheelLeft >> 8);    // Byte 1: high byte of wheel left
    (*msgBuf)[2] = (uint8_t) (args->wheelLeft);         // Byte 2: low byte of wheel left
    (*msgBuf)[3] = (uint8_t) (args->wheelRight >> 8);   // Byte 3: high byte of wheel left
    (*msgBuf)[4] = (uint8_t) (args->wheelRight);        // Byte 4: low byte of wheel left

    return sizeof(CmdMoveRoombaArgs_t) + 1;
}

CmdMoveRoombaArgs_t *CmdMoveRoomba_decode(char const* msgBuf)
{
    CmdMoveRoombaArgs_t *moveArgs = malloc(sizeof(CmdMoveRoombaArgs_t));

    moveArgs->wheelLeft = (msgBuf[1] << 8 | msgBuf[2]);
    moveArgs->wheelRight = (msgBuf[3] << 8 | msgBuf[4]);

    return moveArgs;
}


int8_t Cmd_decode(char const* msgBuf, uint8_t *opcode /*out*/, void **args /*out*/)
{
    *opcode = uart_get_byte(0);
    switch (*opcode)
    {
        case Cmd_MoveRoomba:
        {
            
            uint8_t bufSize = sizeof(CmdMoveRoombaArgs_t) + 1;
            char msgBuf[bufSize];
            while (uart_bytes_received() < bufSize);

            for (int i = 0; i < bufSize; i++)
                msgBuf[i] = uart_get_byte(i);

            *args = (void*) CmdMoveRoomba_decode(msgBuf);
            break;
        }
        default:
            // Unrecognized opcode
            return -1;

    }

    uart_reset_receive();  // clear uart buffer
    return 0;
}

void CmdArgs_free(void *args)
{
    free(args);
}

void CmdMsgBuf_free(char* msgBuf)
{
    free(msgBuf);
}