/* mipslabwork.c

   This file written 2021 by Filip Karlsson 
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */


// DEFINE CONSTANTS
#define TMR2PERIOD ((80000000/256)/100) // bit-clock: 16khz-> bits-trans: 8khz -> samples: 1khz (/8000 and 1:1) (now /128000 8khz) 
#if TMR2PERIOD > 0xffff
#error "Timer period overflow."
#endif

int prime = 112;

const int hi = 0x00;
const int lo = 0x00;

const int DIN = (0x1 << 8);
const int BCLK = (0x2 << 8);
const int WSEL = (0x4 << 8);

unsigned int sound = 0x00;
unsigned int sweep = 0x1;

int holdnote = 0;
int extendcount = 0;
int cyclecount = 0;
int bitcount = 0;
char textstring[] = "text";

void specialdelay(int cyc) {
    volatile int i;
    for (i = cyc; i > 0; i--);
}

/* Interrupt Service Routine */
void user_isr(void) {
    if (extendcount >= 100) {
        if (cyclecount % 2 == 0) {
            PORTDCLR = BCLK; // switch Bit-Clock OFF
            
            if (cyclecount % 16 == 0) {
                // PORTD = !WSEL; // switch Word-Select bit
                if ((PORTD & WSEL) == 0) {
                    PORTDSET = WSEL;
                }
                else {
                    PORTDCLR = WSEL;
                }
                // reset bit selector
                bitcount = 0x80;

                if (cyclecount % 32 == 0) {
                    cyclecount = 0; // marks the end of a double-mono cycle
                }
            }
            else if (cyclecount == 2) {
                // reset bit selector
                //bits = sizeof(uint32) * 8;
                //slct = 1 << (bits - 1);
                bitcount = 0x80;

                // Read&load new sound-int value (will be replaced with reading from microphone)
                if (holdnote >= 0) {
                    if (sound == hi) {
                        sound = lo;
                    }
                    else {
                        sound = hi;
                    }
                    holdnote = 0;
                }
                holdnote++;
                // load in sound
            }

            // set D-in bit (MSB-> LSB)
            if ((bitcount & sound) == 0) {
                PORTDCLR = DIN;
            }
            else {
                PORTDSET = DIN;
            }
            bitcount = bitcount >> 1;

        }
        else {
            PORTDSET = BCLK; // switch Bit-Clock ON
        }
        cyclecount++;
        extendcount = 0;
    }
    extendcount++;

    IFS(0) &= ~0x100; // clear timer2 interrupt flag

    //AD1CON1 |= (0x1 << 1); //SAMP
    //while (!(AD1CON1 & (0x1 << 1))); // SAMP
    //while (!(AD1CON1 & 0x1)); // DONE
    //sound = ADC1BUF0; // not sure how this is related to the thing above^

    // if (300 < sound < 400) { sound = 0;} // to avoid background noise
    
    time2string(textstring, sound);
    display_string(3, textstring);
    time2string(textstring, cyclecount);
    display_string(1, textstring);
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
    T2CONSET = (0x7 << 4); // set prescale value to 1:256 (1:1)
    PR2 = TMR2PERIOD; //  Set period in Hz
    TMR2 = 0; // reset current value
    T2CONSET = (0x1 << 15); // start timer
    enable_interrupt();
    IEC(0) = 0x100; // timer2 interrupt enable
    IPC(2) = 0x1C; // priority

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
}

