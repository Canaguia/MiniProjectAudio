#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */


// player variables

uint8_t currentY = 61;
uint8_t currentX = 15;

int inputcycles;
int playerUpVelocity = 0;
int velocityBuffer = 0;
#define playerUpVelocity_MAX 5
#define playerUpVelocity_MIN -2
#define looney_Toons_Logic 10

// Analog input variables
#define AMP_MIN 0x200 // 220 
#define AMP_MAX 0x320  //300
int analogIn;

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
    /* --- code here ---*/

    int pressedBtn = getbtns();
    
    if (pressedBtn){
        //BTN1
        if(pressedBtn & 1){
            playerMoveUp();
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
    if (analogIn < AMP_MIN || analogIn > AMP_MAX) {
        if (playerUpVelocity < playerUpVelocity_MAX) {
            playerUpVelocity++;
        }    
        inputcycles = 0;
    }
    else {
        if (inputcycles > looney_Toons_Logic) {
            playerUpVelocity--;
            inputcycles = 0;
        }
        inputcycles++;
    }
    
    // Make it so that the maximum speed is 1 pixel per frame (slowest is 1 per 5 frames)
    if (velocityBuffer >= (playerUpVelocity_MAX - playerUpVelocity)) {
        if (playerUpVelocity > 0) {
            currentY--;
        }
        else {
            currentY++;
        }
        
        velocityBuffer = 0;
    }
    velocityBuffer++;
}

void entity_update() {
    /* spawn and update position of existing entities, 
        smart allocation, eg only can have 10 at a time
        (two arrays one with x values and one with y value)*/
    return;
}


// void playerMovement(void){
//     int pressedBtn = getbtns();
    
//     if (pressedBtn){
//         //BTN1
//         if(pressedBtn & 1){
//             playerMoveUp();
//         }
//         // BTN2
//         if(pressedBtn & 2){
//             playerMoveDown();
//         }
//         // BTN3
//         if(pressedBtn & 4){
//             playerMoveLeft();
//         }
//         // BTN4
//         if(pressedBtn & 8){
//             playerMoveRight();
//         }
//         quicksleep(100000);
//     }
    
// }

