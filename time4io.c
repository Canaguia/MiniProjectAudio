#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"


int getsw(void) {
	return (PORTD & 0xf00) >> 8;
}

int getbtns(void) {
	return (((PORTD & 0xe0) >> 4) | ((PORTF & 0x02) >> 1));	
}

