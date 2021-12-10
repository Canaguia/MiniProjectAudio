#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */


// player variables
#define playerSpawn_X 15
#define playerSpawn_Y 80

uint8_t currentY = playerSpawn_Y;
uint8_t currentX = playerSpawn_X;
uint8_t playerLives = 3;

int inputcycles = 0;
int playerUpVelocity = 0;
int velocityBuffer = 0;
#define playerUpVelocity_MAX 10
#define playerUpVelocity_MIN -10
#define coyote_time 50
#define scroll_border 60
#define death_border 122

uint8_t playerInv = 0;
uint8_t playerInvCtr = 0;

// Analog input variables
#define ampBuffer_MAX 10
#define AMP_MIN 150 // 220 
int analogIn = 0x288;
int analogBaseline = 0x288;
int ampVal = 0;
int ampBuffer = 0;

// timer variables
int timerCycles;

int sample_analog(void){
    // Read&load new sound-int value
    AD1CON1 |= (0x1 << 1); //SAMP
    while (!(AD1CON1 & (0x1 << 1))); // SAMP
    while (!(AD1CON1 & 0x1)); // DONE
    return ADC1BUF0;
}

int sample_average(void) {
    int i, a;
    a = 0;
    for (i = 0; i < 100; i++) {
        a += sample_analog();
        quicksleep(100);
    }
    return (a / 100);
}

// update LED's to show new volume w
void led_disp_volume(int max) {
    int a = (((ampVal) * 8) / max);
    int i;
    if (a == 0) {
        return;
    }
    for (i = 0; i < 8; i++) {
        if (a > i) {
            setled(7-i);
        }
        else {
            clearled(7-i);
        }
    }
    return;
}

// takes greatest amplitude in last n-cycles
void aggregate_volume() {
    int a;
    analogIn = sample_analog();
    a = analogIn - analogBaseline;

    // absolute value gives amplitude of wave
    if (a < 0) {
        a = a * -1;
    }

    // if greater than current held value swap
    if (a > ampVal) {
        ampVal = a;
        ampBuffer = 0;
    }
    else if (ampBuffer >= ampBuffer_MAX) {
        ampVal = 0;
    }
    ampBuffer++;

    // display ampVal as string (uncomment for debugging purposes)
    //draw_string(5, 10, itoaconv(ampVal),1);

    led_disp_volume(AMP_MIN);
    return;
}

void playerMoveUp(void){
  if(currentY > 1){
      currentY--;
  }
}

void playerMoveDown(void){
    if(currentY < 120){
        currentY++;
    }
}

void playerMoveRight(void){
  if(currentX < 26){
      currentX++;
  }
}

void playerMoveLeft(void){
    if(currentX > 1){
        currentX--;
    }
}
    
void player_input() {
    //move left, move right
    int pressedBtn = getbtns();
    int a;
    
    if (pressedBtn){
        //BTN1
        if(pressedBtn & 1){
            //playerMoveUp();
        }
        // BTN2
        if(pressedBtn & 2){
            playerMoveDown();
        }
        // BTN3
        if(pressedBtn & 4){
            playerMoveLeft();
        }
        // BTN4
        if(pressedBtn & 8){
            playerMoveRight();
        }
    }

    aggregate_volume();

    if (ampVal > AMP_MIN || pressedBtn & 1) {
        if (playerUpVelocity < playerUpVelocity_MAX) {
            playerUpVelocity++;
            inputcycles = 0;
        }    
    }
    else {
        if ((inputcycles > coyote_time) & (playerUpVelocity > playerUpVelocity_MIN)) {
            playerUpVelocity--;
        }
        inputcycles++;
    }
    
    // Make it so that the maximum speed is 1 pixel per frame (slowest is 1 per 5 frames)
    if (playerUpVelocity > 0) {
        if (velocityBuffer >= (playerUpVelocity_MAX - playerUpVelocity)) {
            if (currentY < scroll_border) {
                generate_walls();
                entity_scroll();
                score++;
            }
            else {
                currentY--;
                velocityBuffer = 0;
            }
        }
    }
    else {
        if (velocityBuffer >= (playerUpVelocity - playerUpVelocity_MIN)) {
            if (currentY > death_border) {
                player_out_of_bounds();
            }
            else if (inputcycles > coyote_time) {
                currentY++;
                velocityBuffer = 0;
            }
        }
    }
    velocityBuffer++;
}

/* deduct life and return to start position */
void player_out_of_bounds() {
    playerLives--;
    playerInv = 1;
    playerInvCtr = 50;
    playerUpVelocity = 0;
    inputcycles = 20;
    currentX = playerSpawn_X;
    currentY = playerSpawn_Y;
    return;
}

