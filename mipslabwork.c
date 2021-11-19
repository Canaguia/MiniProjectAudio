/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */


// DEFINE CONSTANTS
#define TMR2PERIOD (80000000/16000) // bit-clock: 16khz-> bits-trans: 8khz -> samples: 1khz
#if TMR2PERIOD > 0xffff
#error "Timer period overflow."
#endif

const int hi = 0x20;
const int lo = 0x02;

const int DIN = (0x1 << 8);
const int BCLK = (0x2 << 8);
const int WSEL = (0x4 << 8);

unsigned int sound;
unsigned int sweep = 0x1;

int cyclecount = 0;
int bitcount = 0;
char textstring[] = "text";

void specialdelay(int cyc) {
    volatile int i;
    for (i = cyc; i > 0; i--);
}

/* Interrupt Service Routine */
void user_isr(void) {

    if (cyclecount % 2 = 0) {
        PORTDSET = BCLK; // switch Bit-Clock ON

        if (cyclecount % 16 = 0) {
            PORTD = !WSEL; // switch Word-Select bit
            bitcount = 0; // send same output again (mono)

            if (cyclecount % 32 = 0) {
                cyclecount = 0; // marks the end of a double-mono cycle
            }
        }
    }
    else {
        PORTDCLR = BCLK; // switch Bit-Clock OFF

        if (cyclecount = 1) {
            bitcount = 0;

            // load new sound-int value
            AD1CON1 |= (0x1 << 1); //SAMP
            while (!(AD1CON1 & (0x1 << 1))); // SAMP
            while (!(AD1CON1 & 0x1)); // DONE

            sound = ADC1BUF0; // not sure how this is related to the thing above^
        }

        PORTD = DIN; // set D-in bit (MSB-> LSB)
        bitcount++;
    }
    IFS(0) &= ~0x100; // clear timer2 interrupt flag
    cyclecount++;

    // if (300 < sound < 400) { sound = 0;} // to avoid background noise
    time2string(textstring, sound);
    display_string(3, textstring);
    display_update();

    return;
}

void sweeper(void) {
    for (;;) {
        PORTE = sweep;
        if ((sweep <<= 1) >= 0x100)
            sweep = 1;
        specialdelay(sound * 1000);
    }
}

/* Lab-specific initialization goes here */
void labinit(void)
{
    // Establish inputs
    TRISE &= ~0xff;
    PORTE &= ~0xff;
    
    // Establish I2S outputs
    PORTDCLR = (0x7 << 8); // clr RD8,9,10
    TRISD &= ~(0x7 << 8); // set to ^ to output

    // Establish Timer2

    T2CONCLR = (0x1 << 15); // set timer to off
    T2CONCLR = (0x7 << 4); // set prescale value to 1:1
    PR2 = TMR2PERIOD; //  Set period in Hz
    TMR2 = 0; // reset current value
    T2CONSET = (0x1 << 15); // start timer
    enable_interrupt();
    IEC(0) = 0x100; // timer2 interrupt enable
    IPC(2) = 0x1C; // priority

    // set size of input int
    bits = sizeof(uint32) * 8;
    slct = 1 << (bits - 1);
    bitcount = slct;

    // HANDLE ANALOG INPUT

    /* PORTB.4 is analog pin A1*/
	AD1PCFG = ~(1 << 4);
	TRISBSET = (1 << 4);
	/* Use pin 2 for positive */
	AD1CHS = (0x2 << 17); // 16->17 not sure if correct

    /* Data format in uint32, 0 - 1024
    Manual sampling, auto conversion when sampling is done
    FORM = 0x4; SSRC = 0x7; CLRASAM = 0x0; ASAM = 0x0; */
    AD1CON1 = (0x4 << 8) | (0x7 << 5);
    AD1CON2 = 0x0;
    AD1CON3 |= (0x1 << 15);

    /* Set up output pins */
    ODCE = 0x0;
    TRISECLR = 0xFF;

    /* Turn on ADC */
    AD1CON1 |= (0x1 << 15);

    return;
}

/* This function is called repetitively from the main program */
void labwork(void) {
    prime = nextprime(prime);
    display_string(0, itoaconv(prime));
    display_update();
}

