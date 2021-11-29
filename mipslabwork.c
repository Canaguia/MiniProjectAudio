/* mipslabwork.c

   This file written 2021 by Filip Karlsson 
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */


// DEFINE CONSTANTS
#define TMR2PERIOD ((80000000/1)/(8*2*2*16000)) // bit-clock: 16khz-> bits-trans: 8khz -> samples: 1khz (/8000 and 1:1) (now /128000 8khz) (8*2*2*16000)(80000000/256)/100)
#if TMR2PERIOD > 0xffff
#error "Timer period overflow."
#endif

// I2S bus-related constants
const int hi = 50; // 50 in 8-bit
const int lo = 206; //-50 in 8-bit *(twos-complement)
const int DinMSB = 0x80; // 8th bit select
const int I2Sextend = 1;

// Analog output
int frequencyValue = 1000;

//pins 2,7,8 as output
const int DIN = (0x1 << 8); // pin 2
const int BCLK = (0x2 << 8); // pin 7
const int WSEL = (0x4 << 8); // pin 8

// Analog input
unsigned int sound = 0x00;

// Global counters
unsigned int graph_counter = 0; // remove
int holdnote = 0;
int extendcount = 0;
int cyclecount = 0;
int bitcount = 0;
int frequencyCounter = 0;
int sineCounter = 0;

// Graphics related constants
char textstring[] = "text";
uint8_t analogarr[128] = { 0 };
uint8_t I2S_DEBUG = 0;

/* Plot all pixels in the graph and scroll if array is full */
void graph_analog(unsigned int a) {
    int i;
    clear_canvas(); //clears all previously held values in the canvas

    if (graph_counter > 127) {
        for (i = 0; i < 127; i++) {
            analogarr[i] = analogarr[i + 1];
        }
        analogarr[127] =  ((0x3e0 & a) >> 5 ); // only store 5 MSB
    }
    else {
        analogarr[graph_counter] = ((0x3e0 & a) >> 5 ); // only store 5 MSB
        graph_counter++;
    }

    for (i = 0; i < 128; i++) {
        if (analogarr[i] != 0) {
            draw_pixel(i, analogarr[i]);
        }
    }
    display_canvas(); // displays current canvas arr
}

/* Manually sample the 1st value in the */
void manualsample() {
    AD1CON1 |= (0x1 << 1); //SAMP
    while (!(AD1CON1 & (0x1 << 1))); // SAMP
    while (!(AD1CON1 & 0x1)); // DONE
    sound = ADC1BUF0; // not sure how this is related to the thing above^
}

void i2s_output(void) {
    // Debugging mode
    if (I2S_DEBUG) {
        time2string(textstring, sound); //sound
        display_string(3, textstring);
        time2string(textstring, cyclecount); //cycle
        display_string(1, textstring);
        display_update();
        quicksleep(1000000);
    }

    /* Handles I2S bus encoding*/
    if (extendcount >= I2Sextend) {
        if (cyclecount % 2 == 0) {
            PORTDCLR = BCLK; // switch Bit-Clock OFF

            if (cyclecount %16 == 0) {
                // switch Word-Select bit
                if ((PORTD & WSEL) == 0) {
                    PORTDSET = WSEL;
                }
                else {
                    PORTDCLR = WSEL;
                }

                if (cyclecount % 32 == 0) {
                    cyclecount = 0; // marks the end of a double-mono cycle
                }
            }
            else if (cyclecount == 18) {
                // reset bit selector (offset by 2 for LSB and flushing)
                bitcount = DinMSB;
            }
            else if (cyclecount == 2) {
                // reset bit selector
                //bits = sizeof(uint32) * 8;
                //slct = 1 << (bits - 1);
                bitcount = DinMSB;

                
                // Read&load new sound-int value
                AD1CON1 |= (0x1 << 1); //SAMP
                while (!(AD1CON1 & (0x1 << 1))); // SAMP
                while (!(AD1CON1 & 0x1)); // DONE
                sound = ADC1BUF0; // ADC1BUF0
                
                frequencyValue = 100*sound;

                frequencyCounter++;
                if (frequencyCounter >= frequencyValue) {
                    /*
                    sound = sine[sineCounter];
                    sineCounter++;
                    frequencyCounter = 0;
                    if (sineCounter >= 400) {
                        sineCounter = 0;
                    }
                    */
                    if (sound == hi) {
                        sound = lo;
                    }
                    else {
                        sound = hi;
                    }

                }
                // graph values
                // graph_analog(sound);
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
}

/* Interrupt Service Routine */
void user_isr(void) {
    return;
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
    T2CONCLR = (0x7 << 4); // set prescale value to 1:256 (SET) (1:1) (CLR)
    PR2 = TMR2PERIOD; //  Set period in Hz
    TMR2 = 0; // reset current value
    //enable_interrupt(); //Disabled
    //IEC(0) = 0x100; // timer2 interrupt enable
    //IPC(2) = 0x1C; // priority
    T2CONSET = (0x1 << 15); // start timer

    

    // HANDLE ANALOG INPUT

    /* PORTB.4 is analog pin A1*/
	AD1PCFG = ~(1 << 2);
	TRISBSET = (1 << 2);
	/* Use pin 2 for positive */
	AD1CHS = (0x2 << 16); // 16->17

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
    if (TMR2 == 0) {
        i2s_output();
    }
    return;
}
