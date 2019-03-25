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


int8_t Cmd_decode(char const* msgBuf, uint8_t *opcode /*out*/, void **args /*out*/)
{
    *opcode = uart_get_byte(0);
    switch (*opcode)
    {
        case Cmd_MoveRoomba:
        {
            CmdMoveRoombaArgs_t *moveArgs = malloc(sizeof(CmdMoveRoombaArgs_t));

            while (uart_bytes_received() < sizeof(CmdMoveRoombaArgs_t) + 1);
            moveArgs->wheelLeft = (uart_get_byte(1) << 8 | uart_get_byte(2));
            moveArgs->wheelRight = (uart_get_byte(3) << 8 | uart_get_byte(4));

            *args = moveArgs;
            break;
        }
        default:
            // Unrecognized opcode
            return -1;

    }

    uart_reset_receive();  // clear uart buffer
    return 0;
}

