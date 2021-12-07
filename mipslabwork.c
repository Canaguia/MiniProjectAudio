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
int score = 0;
volatile int randomSpice = 439123;

int btnCounter = 2;
uint8_t nameSlotIndex = 0;
uint8_t stateBefore = 2;

int gameOverCtr = 10;
int gameArtCtr = 50;

char playerName[] = "AAA";
char letter0[1] = "A";
char letter1[1] = "A";
char letter2[1] = "A";

/* Interrupt Service Routine */
void user_isr(void) {
    return;
}

/* Returns a pseudorandom int based on the timer value and a given 0-max range (not included)*/
uint8_t pseudo_random(int max) {
    volatile uint8_t x;
    //n = (TMR2 >> 1);
    //k = TMR2 >> 3;
    x = (15032 * randomSpice + 12139) % 929345;
    randomSpice = x;
    x = x % max;
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
        wall_left[127-i] = wall_left[126 - i];
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
    int temp;
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

    /* Turn on ADC */
    AD1CON1 |= (0x1 << 15);

    /* Set up output pins */
    ODCE = 0x0;
    TRISESET = 0xFF;

    /* Set up i2c */
    I2C1CON = 0x0;
    /* I2C Baud rate should be less than 400 kHz, is generated by dividing
    the 40 MHz peripheral bus clock down */
    I2C1BRG = 0x0C2; // set scaling--?
    I2C1STAT = 0x0; // remove ack/etc flags--?
    I2C1CONSET = 1 << 13; //SIDL = 1 (Idle halt)
    I2C1CONSET = 1 << 15; // ON = 1 (turn on)
    temp = I2C1RCV; //Clear receive buffer


    // game init
    populate_walls();
    //clear_highscore_data();

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

void gameArt(void){
    // if(gameArtCtr >= 0){
    //     (((gameArtCtr / 2) % 2 == 1) ? draw_sprite(0,0,32,128,uphi_art) : draw_sprite(0,0,32,128,uphi_art2));
    // } else{
    //     state = 1;
    // }

    // sprite_anim(12, 5, 9, 9, gameArtCtr, 3, shruiken);

    // if(gameArtCtr % 3 == 0){
    //     draw_sprite(12,5,9,9, shruiken);
    // } else if (gameArtCtr % 3 == 1){
    //     draw_sprite(12,5,9,9, shruiken2);
    // } else if (gameArtCtr % 3 == 2){
    //     draw_sprite(12,5,9,9, shruiken3);
    // }

    gameArtCtr--;


}

void gameRunning(void) {
    draw_string(1, 1, itoaconv(score),1);
    // pause logic
    if (getsw() & 1) {
        state = 3;
    }
    
    player_input();

    draw_walls();
    render_entity();

    draw_player(); // important that player is drawn last

    if (playerLives == 0) {
        state = 5;
    }
    return;
}

void gamePaused(void) {

    if (!(getsw() & 1)) {
        state = 2;
    }

    draw_pause(12, 57);
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

    draw_string(9, 10, "AIR",1);
    draw_string(9, 20, "BAL",1);
    draw_string(5, 30, "LOON",1);
    draw_string(5, 116, "BTN1",1);

    draw_play(12, 100);

    if ((getbtns() & 1)) {
        if (tutorialHasBeenShown == 0) {
            state = 4;
            tutorialHasBeenShown = 1;
        }
        else {
            state = 2;
        }
    }

    if (BTN2_PRESSED){
        state = 5;
    }
    if (getsw() & 8) {
        state = 6;
    }
    return;
}

void gameTutorial(void) {
    draw_string(4, 10, "TUTO",1);
    draw_string(4, 20, "RIAL",1);

    if ((getbtns() & 8)) {
        state = 2;
    }

    return;
}

void gameOver(void) {
    draw_string(0, 10, "GAME", 1);
    draw_string(0, 20, "OVER", 1);

    draw_string(0, 40, "SCR", 1);
    draw_string(0, 50, itoaconv(score), 1);

    draw_string(0, 70, "NAME", 1);

    int i;
    for (i = 0; i < 7; i++) {
        draw_pixel(i + 4, 100, 0);
        draw_pixel(i + 14, 100, 0);
        draw_pixel(i + 24, 100, 0);
    }

    if (btnCounter < 0) {
        if (BTN1_PRESSED) {
            switch (nameSlotIndex) {
            case 0:
                letter0[0]++;
                if (letter0[0] > 65 + 25) {
                    letter0[0] = 65;
                }
                break;
            case 1:
                letter1[0]++;
                if (letter1[0] > 65 + 25) {
                    letter1[0] = 65;
                }
                break;
            case 2:
                letter2[0]++;
                if (letter2[0] > 65 + 25) {
                    letter2[0] = 65;
                }
                break;
            default:
                break;
            }
            btnCounter = 2;
        }
        else if (BTN2_PRESSED) {
            nameSlotIndex++;
            if (nameSlotIndex > 2) {
                nameSlotIndex = 0;
            }
            btnCounter = 2;
        }
        else if (BTN3_PRESSED) {
            playerName[0] = letter0[0];
            playerName[1] = letter1[0];
            playerName[2] = letter2[0];

            display_highscores(playerName, score);
        }
        
    }
    btnCounter--;

    if (SW4_SWITCHED) {
        state = 6;
        stateBefore = 5;
    }

    
    if (BTN4_PRESSED) {
        state = 2;
        gameRunningCycle = 0;
        score = 0;
        playerLives = 3;
    }

    if((nameSlotIndex == 0 && gameOverCtr < 5) || nameSlotIndex != 0)
        draw_string(5, 90, letter0, 0);
    if((nameSlotIndex == 1 && gameOverCtr < 5) || nameSlotIndex != 1)
        draw_string(15, 90, letter1, 0);
    if((nameSlotIndex == 2 && gameOverCtr < 5) || nameSlotIndex != 2)
        draw_string(25, 90, letter2, 0);

    gameOverCtr--;
    if(gameOverCtr <= 0){
        gameOverCtr = 10;
    }
    
        draw_string(0, 110, "OK", 1);

    return;
}

void highScores(void) {
    draw_string(5, 0, "TOP5",1);
    // Draw horizontal line under title 
    int i;
    for (i = 0; i < 31; i++) {
        draw_pixel(i, 10, 0);
    }
    display_highscores("NUL", 0);

    if (!(getsw() & 8)) {
        state = 5;
    }
    return;
}

void masterGameLoop() {
    if (TMR2 == 0) {
        clear_canvas();
        if (state == 0){
            gameArt();
        }
        else if (state == 1) {
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
        display_update();
    }
}

