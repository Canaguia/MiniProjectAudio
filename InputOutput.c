#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"


/*  Checks which switches are on */
int getsw(void) {
    return (PORTD & 0xf00) >> 8;
}

/*  Checks which buttons are pressed */
int getbtns(void) {
    return (((PORTD & 0xe0) >> 4) | ((PORTF & 0x02) >> 1));
}

/*  Sets LED at given index */
void setled(uint8_t numLed) {
    PORTE |= (1 << numLed);
}

/* Clears LED at given index */
void clearled(uint8_t numLed) {
    PORTE &= ~(1 << numLed);
}