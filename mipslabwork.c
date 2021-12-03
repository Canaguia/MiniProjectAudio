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


int wall_left[128];
int wall_right[128];
int cycle = 0;

int playerHeight = 0;
int playerFlashDur = 0;

int tutorialHasBeenShown = 0;

int gameStartCycle = 0;
int gameRunningCycle = 0;
int animCycle;
int score;

/* Interrupt Service Routine */
void user_isr(void) {
    return;
}

/* Returns a pseudorandom int based on the timer value and a given 0-max range (not included)*/
int pseudo_random(int max) {
    int n, k, x;
    n = (TMR2 >> 2);
    k = ADC1BUF0 > 3;
    x = (n * k + 35) % max;
    return x;
}

/* Populates the left wall and right wall arrays */
void populate_walls() {
    int i, t;
    for (i = 0; i < 128; i++) {
        t = pseudo_random(2);
        wall_left[i] = t;
        t = pseudo_random(2);
        wall_right[i] = t;
    }
}

/* draw current walls to the canvas */
void draw_walls() {
    int i;
    for (i = 0; i < 128; i++) {
        draw_pixel(0 + wall_left[i], i, 0); // left line
        draw_pixel(31 - wall_right[i], i, 0); // right line
    }
}

/* generate new walls and scroll existing walls downwards */
void generate_walls() {
    int i, t;
    // scroll existing walls by 1 pixel
    for (i = 0; i < 127; i++) {
        wall_left[127-i] = wall_left[126-i];
        wall_right[127 - i] = wall_right[126 - i];
    }
    // generate and insert new wall pattern
    t = pseudo_random(2);
    wall_left[0] = t;
    t = pseudo_random(2);
    wall_right[0] = t;

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

    /* Data format in uint32, 0 - 1024 */
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

void draw_player() {
    // flash current sprite if Invunerable
    if (playerInv) {
        if (playerInvCtr == 0) {
            playerInv = 0;
        }
        else {
            playerInvCtr--;
        }

        playerFlashDur++;
        if (playerFlashDur < 4) {
            return;
        }
        else if (playerFlashDur >= 8) {
            playerFlashDur = 0;
        }
    }

    switch (playerLives) {
    case 3:
        draw_balloon3(currentX, currentY);
        break;
    case 2:
        draw_balloon2(currentX, currentY);
        break;
    case 1:
        draw_balloon1(currentX, currentY);
        break;
    case 0:
        draw_balloon0(currentX,currentY);
        break;
    default:
        break;
    }
    return;
}

void gameRunning(void) {
    // pause logic
    if (SW1_SWITCHED) {
        state = 3;
    }
    draw_walls();

    player_input();

    draw_player();

    draw_bird(1, 80);
    draw_bird(22, 30);

    display_update();

    if(playerLives == 0){
        state = 5;
    }

    return;
}

void gamePaused(void) {

    if (!(SW1_SWITCHED)) {
        state = 2;
    }

    draw_pause(12, 57);
    display_update();

    return;
}

void gameStart(void) {

    animCycle = gameStartCycle / 100;

    if ((animCycle) % 4 == 0) {
        draw_balloon3(15, 61);
    }
    if ((animCycle) % 4 == 1) {
        draw_balloon2(15, 61);
    }
    if ((animCycle) % 4 == 2) {
        draw_balloon1(15, 61);
    }
    if ((animCycle) % 4 == 3) {
        draw_balloon0(14, 60);
    }
    if (animCycle >= 4) {
        animCycle = 0;
    }

    gameStartCycle++;

    draw_string(0, 10, "AIR", 1);
    draw_string(0, 20, "BAL", 1);
    draw_string(0, 30, "LOON", 1);
    draw_string(0, 116, "BTN1", 1);

    draw_play(12, 100);

    display_update();

    if ((BTN1_PRESSED)) {
        if (tutorialHasBeenShown == 0) {
            state = 4;
            tutorialHasBeenShown = 1;
        }
        else {
            state = 2;
        }
    }
    if (SW1_SWITCHED) {
        state = 6;
    }
    return;
}

void gameTutorial(void) {
    draw_string(0, 10, "TUTO", 1);
    draw_string(0, 20, "RIAL", 1);

    display_update();

    if ((getbtns() & 8)) {
        state = 2;
    }

    return;
}

void gameOver(void) {
    draw_string(0, 10, "GAME", 1);
    draw_string(0, 20, "OVER", 1);
    draw_string(0, 40, "SCR", 1);


    if (score / 10 == 0) {
        draw_string(0, 50, itoaconv(score), 1);
    }
    else if (score / 100 == 0) {
        draw_string(0, 50, itoaconv(score), 1);
    }
    else if (score / 1000 == 0) {
        draw_string(0, 50, itoaconv(score), 1);
    }
    else {
        draw_string(0, 50, itoaconv(score), 1);
    }
    draw_play(12, 100);
    draw_string(0, 116, "BTN1", 1);

    if (BTN1_PRESSED) {
        state = 2;
        gameRunningCycle = 0;
    }

    if (SW4_SWITCHED) {
        state = 6;
    } else {
        state = 5;
    }
    display_update();
}

void highScores(void) {
    draw_string(0, 0, "TOP5", 1);
    // Draw horizontal line under title 
    int i;
    for (i = 0; i < 31; i++) {
        draw_pixel(i, 10, 0);
    }

    draw_string(0, 12, "ASS", 0);
    draw_string(0, 20, itoaconv(999), 1);
    draw_string(0, 32, "SNA", 1);
    draw_string(0, 40, itoaconv(528), 1);
    draw_string(0, 52, "FLP", 1);
    draw_string(0, 60, itoaconv(527), 1);
    draw_string(0, 72, "LOL", 1);
    draw_string(0, 80, itoaconv(75), 1);
    draw_string(0, 92, "AAA", 1);
    draw_string(0, 100, itoaconv(11), 1);

    display_update();
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
        else if (state == 4) {
            gameTutorial();
        }
        else if (state == 5) {
            gameOver();
        }
        else if (state == 6) {
            highScores();
        }
    }
}

