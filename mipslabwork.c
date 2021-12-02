/* mipslabwork.c

   This file written 2021 by Filip Karlsson 
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

// DEFINE CONSTANTS
#define TMR2PERIOD ((80000000/256)/(20)) // bit-clock: 16khz-> bits-trans: 8khz -> samples: 1khz (/8000 and 1:1) (now /128000 8khz) (8*2*2*16000)(80000000/256)/100)
#if TMR2PERIOD > 0xffff
#error "Timer period overflow."
#endif

int cycle = 0;

/* Interrupt Service Routine */
void user_isr(void) {
    return;
}

/* Lab-specific initialization goes here */
void labinit(void)
{
    // Establish Timer2
    T2CONCLR = (0x1 << 15); // set timer to off
    T2CONSET = (0x7 << 4); // set prescale value to 1:256 (SET) (1:1) (CLR)
    PR2 = TMR2PERIOD; //  Set period in Hz
    TMR2 = 0; // reset current value
    T2CONSET = (0x1 << 15); // start timer

    // HANDLE ANALOG INPUT
    /* PORTB.4 is analog pin A1*/
	AD1PCFG = ~(1 << 4);
	TRISBSET = (1 << 4);
	/* Use pin 2 for positive */
	AD1CHS = (0x2 << 17); // 16->17

    /* Data format in uint32, 0 - 1024
    Manual sampling, auto conversion when sampling is done
    FORM = 0x4; SSRC = 0x7; CLRASAM = 0x0; ASAM = 0x0; */
    AD1CON1 = (0x4 << 8) | (0x7 << 5);
    AD1CON2 = 0x0;
    AD1CON3 |= (0x1 << 15);

    /* Set up output pins */
    ODCE = 0x0;
    TRISESET = 0xFF;

    /* Turn on ADC */
    AD1CON1 |= (0x1 << 15);

    return;
}

void gameRunning(void) {
    // pause logic
    if (getsw() & 1) {
        state = 3;
    }

    int i;
    for (i = 1; i < 128; i++) {
        draw_pixel(0, i, 0); // left line
        draw_pixel(31, i, 0); // right line
    }

    player_input();

    draw_balloon3(currentX, currentY);

    draw_bird(1, 80);
    draw_bird(22, 30);

    display_update();

    return;
}

void gamePaused(void) {

    if (!(getsw() & 1)) {
        state = 2;
    }

    draw_pause(12, 57);
    display_update();

    return;
}

void gameStart(void) {

    if ((cycle / 100) % 4 == 0) {
        draw_balloon3(15, 61);
    }
    if ((cycle / 100) % 4 == 1) {
        draw_balloon2(15, 61);
    }
    if ((cycle / 100) % 4 == 2) {
        draw_balloon1(15, 61);
    }
    if ((cycle / 100) % 4 == 3) {
        draw_balloon0(14, 60);
    }
    if (cycle >= 400) {
        cycle = 0;
    }
    cycle++;

    draw_string(10, 20, "SINA");

    draw_play(12, 100);

    display_update();

    quicksleep(1000);

    if ((getbtns() & 1)) {
        state = 2;
    }

    return;
}

void masterGameLoop() {
    if (TMR2 == 0) {
        clear_canvas();
        if (state == 1) {
            gameStart();
        }
        else if (state == 2) {
            gameRunning();
        }
        else if (state == 3) {
            gamePaused();
        }
    }
}

