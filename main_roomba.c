/**
 * main_roomba.c
 * Roomba program entry point.
 */

#include "roomba/roomba.h"

int main() {
	Roomba_Init();
	Roomba_Drive(100, 0);

	while (1);
}
