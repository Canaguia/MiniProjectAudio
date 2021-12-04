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
#define playerUpVelocity_MAX 3
#define playerUpVelocity_MIN -3
#define coyote_time 30
#define scroll_border 60
#define death_border 122

uint8_t playerInv = 0;
uint8_t playerInvCtr = 0;

// Analog input variables
#define AMP_MIN 300 // 220 
#define AMP_MAX 600  //300
int analogIn = 0x288;

// timer variables
int timerCycles;

int sample_analog(void){
    // Read&load new sound-int value
    AD1CON1 |= (0x1 << 1); //SAMP
    while (!(AD1CON1 & (0x1 << 1))); // SAMP
    while (!(AD1CON1 & 0x1)); // DONE
    return ADC1BUF0;
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
    analogIn = sample_analog();
    draw_string(5, 10, itoaconv(analogIn));

    if (analogIn < AMP_MIN || analogIn > AMP_MAX) {
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
            else {
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
    currentX = playerSpawn_X;
    currentY = playerSpawn_Y;
    return;
}

