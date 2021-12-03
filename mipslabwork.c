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

int tutorialHasBeenShown = 0;

int gameStartCycle = 0;
int gameRunningCycle = 0;
int animCycle;
int score;

// I2S bus-related constants
// const int hi = 0x3FF;
// const int lo = 0x1FF;
// const int DinMSB = 0x200;
// const int I2Sextend = 1;

// //pins 2,7,8 as output
// const int DIN = (0x1 << 8);
// const int BCLK = (0x2 << 8);
// const int WSEL = (0x4 << 8);


// Analog input
// unsigned int sound = 0x00;

// // Global counters
// unsigned int graph_counter = 0; // remove
// int holdnote = 0;
// int extendcount = 0;
// int cyclecount = 0;
// int bitcount = 0;
// int frequencyCounter = 0;
// int frequencyValue = 1000;

// // Graphics related constants
// char textstring[] = "text";
// uint8_t analogarr[128] = { 0 };
// uint8_t I2S_DEBUG = 0;

/* Plot all pixels in the graph and scroll if array is full */
// void graph_analog(unsigned int a) {
//     int i;
//     clear_canvas(); //clears all previously held values in the canvas

//     if (graph_counter > 127) {
//         for (i = 0; i < 127; i++) {
//             analogarr[i] = analogarr[i + 1];
//         }
//         analogarr[127] =  ((0x3e0 & a) >> 5 ); // only store 5 MSB
//     }
//     else {
//         analogarr[graph_counter] = ((0x3e0 & a) >> 5 ); // only store 5 MSB
//         graph_counter++;
//     }

//     for (i = 0; i < 128; i++) {
//         if (analogarr[i] != 0) {
//             draw_pixel(i, analogarr[i], 0);
//         }
//     }
//     display_canvas(); // displays current canvas arr
// }

/* Manually sample the 1st value in the */
// void manualsample() {
//     AD1CON1 |= (0x1 << 1); //SAMP
//     while (!(AD1CON1 & (0x1 << 1))); // SAMP
//     while (!(AD1CON1 & 0x1)); // DONE
//     sound = ADC1BUF0; // not sure how this is related to the thing above^
// }

// void i2s_output(void) {
//     // Debugging mode
//     if (I2S_DEBUG) {
//         time2string(textstring, sound); //sound
//         display_string(3, textstring);
//         time2string(textstring, cyclecount); //cycle
//         display_string(1, textstring);
//         display_update();
//         quicksleep(1000000);
//     }


//     /* Handles I2S bus encoding*/
//     if (extendcount >= I2Sextend) {
//         if (cyclecount % 2 == 0) {
//             PORTDCLR = BCLK; // switch Bit-Clock OFF

//             if (cyclecount % 16 == 0) {
//                 // PORTD = !WSEL; // switch Word-Select bit
//                 if ((PORTD & WSEL) == 0) {
//                     PORTDSET = WSEL;
//                 }
//                 else {
//                     PORTDCLR = WSEL;
//                 }
//                 // reset bit selector
//                 bitcount = DinMSB;

//                 if (cyclecount % 32 == 0) {
//                     cyclecount = 0; // marks the end of a double-mono cycle
//                 }
//             }
//             else if (cyclecount == 2) {
//                 // reset bit selector
//                 //bits = sizeof(uint32) * 8;
//                 //slct = 1 << (bits - 1);
//                 bitcount = DinMSB;

                



                
//                 frequencyValue = 1000*sound;

//                 frequencyCounter++;
//                 if (frequencyCounter >= frequencyValue) {
//                     if (sound == hi) {
//                         sound = lo;
//                     }
//                     else {
//                         sound = hi;
//                     }
//                     frequencyCounter = 0;
//                 }

                 

//                 // graph values
//                 //graph_analog(sound);
//             }

//             // set D-in bit (MSB-> LSB)
//             if ((bitcount & sound) == 0) {
//                 PORTDCLR = DIN;
//             }
//             else {
//                 PORTDSET = DIN;
//             }
//             bitcount = bitcount >> 1;

//         }
//         else {
//             PORTDSET = BCLK; // switch Bit-Clock ON
//         }
//         cyclecount++;
//         extendcount = 0;
//     }
//     extendcount++;
// }

/* Interrupt Service Routine */
void user_isr(void) {
    return;
}

/* Lab-specific initialization goes here */
void labinit(void)
{
    // Establish inputs
    // TRISE &= ~0xff;
    // PORTE &= ~0xff;

    // Establish I2S outputs
    // PORTDCLR = (0x7 << 8); // clr RD8,9,10
    // TRISD &= ~(0x7 << 8); // set to ^ to output


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

    score = gameRunningCycle/50;

    if(getsw() & 1){
        state = 3;
    }

    if(getbtns() & 2){
        state = 5;
    }

    int i;
    for (i = 1; i < 128; i++) {
        draw_pixel(0, i, 0); // left line
        draw_pixel(31, i, 0); // right line
    }

    player_input();

    draw_balloon3(currentX, currentY);

    if(score / 10 == 0){
        draw_string(14, 0, itoaconv(score));
    } else if (score / 100 == 0){
        draw_string(11, 0, itoaconv(score));
    } else if (score / 1000 == 0){
        draw_string(8, 0, itoaconv(score));
    } else {
        draw_string(5, 0, itoaconv(score));
    }

    draw_bird(1, 80);
    draw_bird(22, 30);

    display_update();

    gameRunningCycle++;

    return;
}

void gamePaused(void) {

    if(!(getsw() & 1)){
        state = 2;
    }

    draw_pause(12, 57);
    display_update();


    return;
}

void gameStart(void) {

    animCycle = gameStartCycle/100;

    if((animCycle) % 4 == 0){
        draw_balloon3(15, 61);
    }
    if((animCycle) % 4 == 1){
        draw_balloon2(15, 61);
    }
    if((animCycle) % 4 == 2){
        draw_balloon1(15, 61);
    }
    if((animCycle) % 4 == 3){
        draw_balloon0(14, 60);
    }
    if(animCycle >= 4){
        animCycle = 0;
    }

    gameStartCycle++;

    

    // draw_string(10, 1, "SINA");

    draw_string(9, 10, "AIR");
    draw_string(9, 20, "BAL");
    draw_string(5, 30, "LOON");
    draw_string(5, 116, "BTN1");

    draw_play(12, 100);

    display_update();

    quicksleep(1000);

    if((getbtns() & 1)){
        if(tutorialHasBeenShown == 0){
            state = 4;
            tutorialHasBeenShown = 1;
        }
        else{
            state = 2;
        }
    }
    if(getsw() & 8){
        state = 6;
    }
    return;
}

void gameTutorial(void){
    draw_string(4, 10, "TUTO");
    draw_string(4, 20, "RIAL");
    
    display_update();

    if((getbtns() & 8)){
        state = 2;
    }

    return;
}

void gameOver(void){
    draw_string(4,10, "GAME");
    draw_string(4,20, "OVER");
    draw_string(8,40, "SCR");

    
    if(score / 10 == 0){
        draw_string(14, 50, itoaconv(score));
    } else if (score / 100 == 0){
        draw_string(11, 50, itoaconv(score));
    } else if (score / 1000 == 0){
        draw_string(8, 50, itoaconv(score));
    } else {
        draw_string(5, 50, itoaconv(score));
    }
    draw_play(12, 100);
    draw_string(5, 116, "BTN1");

    if(getbtns() & 1){
        state = 2;
        gameRunningCycle = 0;
    }

    if(getsw() & 8){
        state = 6;
    }
    display_update();
}

void highScores(void){
    draw_string(5, 0, "TOP5");
    // Draw horizontal line under title 
    int i;
    for(i = 0; i < 31; i++){
        draw_pixel(i, 10, 0); 
    }

    draw_string(8, 12, "ASS");
    draw_string(10, 20, itoaconv(999));
    draw_string(8, 32, "SNA");
    draw_string(10, 40, itoaconv(528));
    draw_string(8, 52, "FLP");
    draw_string(10, 60, itoaconv(527));
    draw_string(8, 72, "LOL");
    draw_string(10, 80, itoaconv(75));
    draw_string(8, 92, "AAA");
    draw_string(10, 100, itoaconv(11));

    display_update();
}