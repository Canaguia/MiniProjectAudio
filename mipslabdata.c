/* mipslabdata.c
   This file compiled 2015 by F Lundevall
   from original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

char textbuffer[4][16];

const uint8_t const font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 94, 0, 0, 0, 0,
	0, 0, 4, 3, 4, 3, 0, 0,
	0, 36, 126, 36, 36, 126, 36, 0,
	0, 36, 74, 255, 82, 36, 0, 0,
	0, 70, 38, 16, 8, 100, 98, 0,
	0, 52, 74, 74, 52, 32, 80, 0,
	0, 0, 0, 4, 3, 0, 0, 0,
	0, 0, 0, 126, 129, 0, 0, 0,
	0, 0, 0, 129, 126, 0, 0, 0,
	0, 42, 28, 62, 28, 42, 0, 0,
	0, 8, 8, 62, 8, 8, 0, 0,
	0, 0, 0, 128, 96, 0, 0, 0,
	0, 8, 8, 8, 8, 8, 0, 0,
	0, 0, 0, 0, 96, 0, 0, 0,
	0, 64, 32, 16, 8, 4, 2, 0,
	0, 62, 65, 73, 65, 62, 0, 0,
	0, 0, 66, 127, 64, 0, 0, 0,
	0, 0, 98, 81, 73, 70, 0, 0,
	0, 0, 34, 73, 73, 54, 0, 0,
	0, 0, 14, 8, 127, 8, 0, 0,
	0, 0, 35, 69, 69, 57, 0, 0,
	0, 0, 62, 73, 73, 50, 0, 0,
	0, 0, 1, 97, 25, 7, 0, 0,
	0, 0, 54, 73, 73, 54, 0, 0,
	0, 0, 6, 9, 9, 126, 0, 0,
	0, 0, 0, 102, 0, 0, 0, 0,
	0, 0, 128, 102, 0, 0, 0, 0,
	0, 0, 8, 20, 34, 65, 0, 0,
	0, 0, 20, 20, 20, 20, 0, 0,
	0, 0, 65, 34, 20, 8, 0, 0,
	0, 2, 1, 81, 9, 6, 0, 0,
	0, 28, 34, 89, 89, 82, 12, 0,
	0, 0, 126, 9, 9, 126, 0, 0,
	0, 0, 127, 73, 73, 54, 0, 0,
	0, 0, 62, 65, 65, 34, 0, 0,
	0, 0, 127, 65, 65, 62, 0, 0,
	0, 0, 127, 73, 73, 65, 0, 0,
	0, 0, 127, 9, 9, 1, 0, 0,
	0, 0, 62, 65, 81, 50, 0, 0,
	0, 0, 127, 8, 8, 127, 0, 0,
	0, 0, 65, 127, 65, 0, 0, 0,
	0, 0, 32, 64, 64, 63, 0, 0,
	0, 0, 127, 8, 20, 99, 0, 0,
	0, 0, 127, 64, 64, 64, 0, 0,
	0, 127, 2, 4, 2, 127, 0, 0,
	0, 127, 6, 8, 48, 127, 0, 0,
	0, 0, 62, 65, 65, 62, 0, 0,
	0, 0, 127, 9, 9, 6, 0, 0,
	0, 0, 62, 65, 97, 126, 64, 0,
	0, 0, 127, 9, 9, 118, 0, 0,
	0, 0, 38, 73, 73, 50, 0, 0,
	0, 1, 1, 127, 1, 1, 0, 0,
	0, 0, 63, 64, 64, 63, 0, 0,
	0, 31, 32, 64, 32, 31, 0, 0,
	0, 63, 64, 48, 64, 63, 0, 0,
	0, 0, 119, 8, 8, 119, 0, 0,
	0, 3, 4, 120, 4, 3, 0, 0,
	0, 0, 113, 73, 73, 71, 0, 0,
	0, 0, 127, 65, 65, 0, 0, 0,
	0, 2, 4, 8, 16, 32, 64, 0,
	0, 0, 0, 65, 65, 127, 0, 0,
	0, 4, 2, 1, 2, 4, 0, 0,
	0, 64, 64, 64, 64, 64, 64, 0,
	0, 0, 1, 2, 4, 0, 0, 0,
	0, 0, 48, 72, 40, 120, 0, 0,
	0, 0, 127, 72, 72, 48, 0, 0,
	0, 0, 48, 72, 72, 0, 0, 0,
	0, 0, 48, 72, 72, 127, 0, 0,
	0, 0, 48, 88, 88, 16, 0, 0,
	0, 0, 126, 9, 1, 2, 0, 0,
	0, 0, 80, 152, 152, 112, 0, 0,
	0, 0, 127, 8, 8, 112, 0, 0,
	0, 0, 0, 122, 0, 0, 0, 0,
	0, 0, 64, 128, 128, 122, 0, 0,
	0, 0, 127, 16, 40, 72, 0, 0,
	0, 0, 0, 127, 0, 0, 0, 0,
	0, 120, 8, 16, 8, 112, 0, 0,
	0, 0, 120, 8, 8, 112, 0, 0,
	0, 0, 48, 72, 72, 48, 0, 0,
	0, 0, 248, 40, 40, 16, 0, 0,
	0, 0, 16, 40, 40, 248, 0, 0,
	0, 0, 112, 8, 8, 16, 0, 0,
	0, 0, 72, 84, 84, 36, 0, 0,
	0, 0, 8, 60, 72, 32, 0, 0,
	0, 0, 56, 64, 32, 120, 0, 0,
	0, 0, 56, 64, 56, 0, 0, 0,
	0, 56, 64, 32, 64, 56, 0, 0,
	0, 0, 72, 48, 48, 72, 0, 0,
	0, 0, 24, 160, 160, 120, 0, 0,
	0, 0, 100, 84, 84, 76, 0, 0,
	0, 0, 8, 28, 34, 65, 0, 0,
	0, 0, 0, 126, 0, 0, 0, 0,
	0, 0, 65, 34, 28, 8, 0, 0,
	0, 0, 4, 2, 4, 2, 0, 0,
	0, 120, 68, 66, 68, 120, 0, 0,
};

const uint8_t const icon[] = {
	255, 255, 255, 255, 255, 255, 127, 187,
	68, 95, 170, 93, 163, 215, 175, 95,
	175, 95, 175, 95, 223, 111, 175, 247,
	59, 237, 242, 254, 171, 254, 1, 255,
	255, 255, 15, 211, 109, 58, 253, 8,
	178, 77, 58, 199, 122, 197, 242, 173,
	242, 237, 186, 215, 40, 215, 41, 214,
	35, 175, 91, 212, 63, 234, 149, 111,
	171, 84, 253, 252, 254, 253, 126, 184,
	195, 52, 201, 22, 225, 27, 196, 19,
	165, 74, 36, 146, 72, 162, 85, 8,
	226, 25, 166, 80, 167, 216, 167, 88,
	106, 149, 161, 95, 135, 91, 175, 87,
	142, 123, 134, 127, 134, 121, 134, 121,
	132, 59, 192, 27, 164, 74, 177, 70,
	184, 69, 186, 69, 254, 80, 175, 217,
};

const uint8_t const icon2[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x02,0x02,0x02,0x03,0x00,0x00,0x00,0x00,0x00,0x03,0x06,0x08,0x0f,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xc0,0x40,0x40,0x40,0xc0,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0c,0x18,0x10,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x30,0x10,0x18,0x0c,0x06,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

const uint8_t const balloon[] = { 0x0e,0x1f,0x1f,0x0e,0x04,0x0a,0x0e };

/* Array for all the letters, numbers and stuff. */

// Chars are displayed in 5x8
// Each row represents a char.
// Each column represents the char column that has 8 pixels/bits.
// Left-most bit is the top-most pixel
// Right-most bit is the bottom-most pixel
const uint8_t const charArray[185] = {
    0xFC, 0x12, 0x12, 0x12, 0xFC, // A
    0xFE, 0x92, 0x92, 0x92, 0x6C, // B
    0x7C, 0x82, 0x82, 0x82, 0x44, // C
    0xFE, 0x82, 0x82, 0x82, 0x7C, // D
    0xFE, 0x92, 0x92, 0x92, 0x92, // E
    0xFE, 0x12, 0x12, 0x12, 0x2,  // F
    0x7C, 0x82, 0x82, 0xA2, 0x64, // G
    0xFE, 0x10, 0x10, 0x10, 0xFE, // H
    0x0, 0x82, 0xFE, 0x82, 0x0,   // I
    0x40, 0x80, 0x82, 0x7E, 0x2,  // J

    0x0, 0x0, 0x0, 0x0, 0x0, // K

    0xFE, 0x80, 0x80, 0x80, 0x80, // L
    0xFE, 0x4, 0x18, 0x4, 0xFE,   // M
    0xFE, 0x8, 0x10, 0x20, 0xFE,  // N
    0x7C, 0x82, 0x82, 0x82, 0x7C, // O


    0xFE, 0x12, 0x12, 0x12, 0x0C, // P 

    0x0, 0x0, 0x0, 0x0, 0x0, // Q

    0xFE, 0x12, 0x12, 0x12, 0xEC, // R
    0x8C, 0x92, 0x92, 0x92, 0x62, // S
    0x2, 0x2, 0xFE, 0x2, 0x2,     // T
    0x7E, 0x80, 0x80, 0x80, 0x7E, // U
    0x1E, 0x60, 0x80, 0x60, 0x1E, // V

    0xFF, 0x81, 0x81, 0x81, 0xFF, // W // YOU ARE HERE!!!!

    0x0, 0x0, 0x0, 0x0, 0x0, // X
    0x6, 0x8, 0xF0, 0x8, 0x6, // Y
    0x0, 0x0, 0x0, 0x0, 0x0, // Z
    0x7C, 0xA2, 0x92, 0x8A, 0x7C, // 0
    0x0, 0x84, 0xFE, 0x80, 0x0,   // 1
    0x84, 0xC2, 0xA2, 0x92, 0x8C, // 2
    0x42, 0x82, 0x8A, 0x96, 0x62, // 3
    0x30, 0x28, 0x24, 0xFE, 0x20, // 4
    0x4E, 0x8A, 0x8A, 0x8A, 0x7A, // 5
    0x78, 0x94, 0x92, 0x92, 0x60, // 6
    0x2, 0xE2, 0x12, 0xA, 0x6,    // 7
    0x6C, 0x92, 0x92, 0x92, 0x6C, // 8
    0xC, 0x92, 0x92, 0x52, 0x3C,  // 9
    0x6C, 0x6C, 0x0, 0x0, 0x0     // :
};

// Baloon with 3HP
void draw_balloon3(int x, int y){
	draw_pixel(x+1, y  , 1);
	draw_pixel(x+2, y  , 1);
	draw_pixel(x+3, y  , 1);
	draw_pixel(x,   y+1, 1);
	draw_pixel(x+1, y+1, 1);
	draw_pixel(x+2, y+1, 1);
	draw_pixel(x+3, y+1, 1);
	draw_pixel(x+4, y+1, 1);
	draw_pixel(x,   y+2, 1);
	draw_pixel(x+1, y+2, 1);
	draw_pixel(x+2, y+2, 1);
	draw_pixel(x+3, y+2, 1);
	draw_pixel(x+4, y+2, 1);
	draw_pixel(x+1, y+3, 1);
	draw_pixel(x+2, y+3, 1);
	draw_pixel(x+3, y+3, 1);
	draw_pixel(x+2, y+4, 1);
	draw_pixel(x+2, y+4, 1);
	draw_pixel(x+1, y+5, 1);
	draw_pixel(x+3, y+5, 1);
	draw_pixel(x+1, y+6, 1);
	draw_pixel(x+2, y+6, 1);
	draw_pixel(x+3, y+6, 1);
}

// Baloon with 2HP
void draw_balloon2(int x, int y){
	draw_pixel(x+1, y  , 1);
	draw_pixel(x+2, y  , 1);
	draw_pixel(x+3, y  , 1);
	draw_pixel(x,   y+1, 1);
	draw_pixel(x+4, y+1, 1);
	draw_pixel(x,   y+2, 1);
	draw_pixel(x+1, y+2, 1);
	draw_pixel(x+2, y+2, 1);
	draw_pixel(x+3, y+2, 1);
	draw_pixel(x+4, y+2, 1);
	draw_pixel(x+1, y+3, 1);
	draw_pixel(x+2, y+3, 1);
	draw_pixel(x+3, y+3, 1);
	draw_pixel(x+2, y+4, 1);
	draw_pixel(x+2, y+4, 1);
	draw_pixel(x+1, y+5, 1);
	draw_pixel(x+3, y+5, 1);
	draw_pixel(x+1, y+6, 1);
	draw_pixel(x+2, y+6, 1);
	draw_pixel(x+3, y+6, 1);
}

// Baloon with 1HP
void draw_balloon1(int x, int y){
	draw_pixel(x+1, y  , 1);
	draw_pixel(x+2, y  , 1);
	draw_pixel(x+3, y  , 1);
	draw_pixel(x,   y+1, 1);
	draw_pixel(x+4, y+1, 1);
	draw_pixel(x,   y+2, 1);
	draw_pixel(x+4, y+2, 1);
	draw_pixel(x+1, y+3, 1);
	draw_pixel(x+3, y+3, 1);
	draw_pixel(x+2, y+4, 1);
	draw_pixel(x+2, y+4, 1);
	draw_pixel(x+1, y+5, 1);
	draw_pixel(x+3, y+5, 1);
	draw_pixel(x+1, y+6, 1);
	draw_pixel(x+2, y+6, 1);
	draw_pixel(x+3, y+6, 1);
}

// Baloon POPPING 0HP!
void draw_balloon0(int x, int y){
	draw_pixel(x+1, y  , 1);
	draw_pixel(x+5, y  , 1);
	draw_pixel(x,   y+2, 1);
	draw_pixel(x+3, y+2, 1);
	draw_pixel(x+6, y+2, 1);
	draw_pixel(x+1, y+4, 1);
	draw_pixel(x+3, y+4, 1);
	draw_pixel(x+5, y+4, 1);
	draw_pixel(x+2, y+5, 1);
	draw_pixel(x+4, y+5, 1);
	draw_pixel(x+2, y+6, 1);
	draw_pixel(x+3, y+6, 1);
	draw_pixel(x+4, y+6, 1);
}

void draw_balloonD(int x, int y) {
	draw_pixel(x + 3, y + 4, 1);
	draw_pixel(x + 2, y + 5, 1);
	draw_pixel(x + 4, y + 5, 1);
	draw_pixel(x + 2, y + 6, 1);
	draw_pixel(x + 3, y + 6, 1);
	draw_pixel(x + 4, y + 6, 1);
}

// Baloon POPPING 0HP!
void draw_bird(int x, int y){
	draw_pixel(x+3, y  , 0);
	draw_pixel(x+4, y  , 0);
	draw_pixel(x+5, y  , 0);
	draw_pixel(x+6, y  , 0);

	draw_pixel(x+2, y+1, 0);
	draw_pixel(x+3, y+1, 0);
	draw_pixel(x+5, y+1, 0);

	draw_pixel(x+3, y+2, 0);
	draw_pixel(x+4, y+2, 0);
	draw_pixel(x+5, y+2, 0);

	draw_pixel(x+6, y+3, 0);

	draw_pixel(x+4, y+4, 0);
	draw_pixel(x+5, y+4, 0);
	draw_pixel(x+6, y+4, 0);
	draw_pixel(x+7, y+4, 0);

	draw_pixel(x  , y+5, 0);
	draw_pixel(x+4, y+5, 0);
	draw_pixel(x+5, y+5, 0);
	draw_pixel(x+6, y+5, 0);
	draw_pixel(x+7, y+5, 0);

	draw_pixel(x  , y+6, 0);
	draw_pixel(x+1, y+6, 0);
	draw_pixel(x+4, y+6, 0);
	draw_pixel(x+5, y+6, 0);
	draw_pixel(x+6, y+6, 0);

	draw_pixel(x  , y+7, 0);
	draw_pixel(x+1, y+7, 0);
	draw_pixel(x+5, y+7, 0);

	draw_pixel(x+1, y+8, 0);
	draw_pixel(x+2, y+8, 0);
	draw_pixel(x+4, y+8, 0);
	draw_pixel(x+6, y+8, 0);
	draw_pixel(x+8, y+8, 0);
	draw_pixel(x+9, y+8, 0);
	
	draw_pixel(x+7, y+9, 0);
}


void draw_pause(int x, int y){
	draw_pixel(x   , y  , 0);
	draw_pixel(x+1 , y  , 0);
	draw_pixel(x+2 , y  , 0);
	draw_pixel(x+3 , y  , 0);
	draw_pixel(x+7 , y  , 0);
	draw_pixel(x+8 , y  , 0);
	draw_pixel(x+9 , y  , 0);
	draw_pixel(x+10, y  , 0);

	draw_pixel(x   , y+1, 0);
	draw_pixel(x+1 , y+1, 0);
	draw_pixel(x+2 , y+1, 0);
	draw_pixel(x+3 , y+1, 0);
	draw_pixel(x+7 , y+1, 0);
	draw_pixel(x+8 , y+1, 0);
	draw_pixel(x+9 , y+1, 0);
	draw_pixel(x+10, y+1, 0);

	draw_pixel(x   , y+2, 0);
	draw_pixel(x+1 , y+2, 0);
	draw_pixel(x+2 , y+2, 0);
	draw_pixel(x+3 , y+2, 0);
	draw_pixel(x+7 , y+2, 0);
	draw_pixel(x+8 , y+2, 0);
	draw_pixel(x+9 , y+2, 0);
	draw_pixel(x+10, y+2, 0);

	draw_pixel(x   , y+3, 0);
	draw_pixel(x+1 , y+3, 0);
	draw_pixel(x+2 , y+3, 0);
	draw_pixel(x+3 , y+3, 0);
	draw_pixel(x+7 , y+3, 0);
	draw_pixel(x+8 , y+3, 0);
	draw_pixel(x+9 , y+3, 0);
	draw_pixel(x+10, y+3, 0);

	draw_pixel(x   , y+4, 0);
	draw_pixel(x+1 , y+4, 0);
	draw_pixel(x+2 , y+4, 0);
	draw_pixel(x+3 , y+4, 0);
	draw_pixel(x+7 , y+4, 0);
	draw_pixel(x+8 , y+4, 0);
	draw_pixel(x+9 , y+4, 0);
	draw_pixel(x+10, y+4, 0);

	draw_pixel(x   , y+5, 0);
	draw_pixel(x+1 , y+5, 0);
	draw_pixel(x+2 , y+5, 0);
	draw_pixel(x+3 , y+5, 0);
	draw_pixel(x+7 , y+5, 0);
	draw_pixel(x+8 , y+5, 0);
	draw_pixel(x+9 , y+5, 0);
	draw_pixel(x+10, y+5, 0);

	draw_pixel(x   , y+6, 0);
	draw_pixel(x+1 , y+6, 0);
	draw_pixel(x+2 , y+6, 0);
	draw_pixel(x+3 , y+6, 0);
	draw_pixel(x+7 , y+6, 0);
	draw_pixel(x+8 , y+6, 0);
	draw_pixel(x+9 , y+6, 0);
	draw_pixel(x+10, y+6, 0);

	draw_pixel(x   , y+7, 0);
	draw_pixel(x+1 , y+7, 0);
	draw_pixel(x+2 , y+7, 0);
	draw_pixel(x+3 , y+7, 0);
	draw_pixel(x+7 , y+7, 0);
	draw_pixel(x+8 , y+7, 0);
	draw_pixel(x+9 , y+7, 0);
	draw_pixel(x+10, y+7, 0);

	draw_pixel(x   , y+8, 0);
	draw_pixel(x+1 , y+8, 0);
	draw_pixel(x+2 , y+8, 0);
	draw_pixel(x+3 , y+8, 0);
	draw_pixel(x+7 , y+8, 0);
	draw_pixel(x+8 , y+8, 0);
	draw_pixel(x+9 , y+8, 0);
	draw_pixel(x+10, y+8, 0);

	draw_pixel(x   , y+9, 0);
	draw_pixel(x+1 , y+9, 0);
	draw_pixel(x+2 , y+9, 0);
	draw_pixel(x+3 , y+9, 0);
	draw_pixel(x+7 , y+9, 0);
	draw_pixel(x+8 , y+9, 0);
	draw_pixel(x+9 , y+9, 0);
	draw_pixel(x+10, y+9, 0);

	draw_pixel(x   , y+10, 0);
	draw_pixel(x+1 , y+10, 0);
	draw_pixel(x+2 , y+10, 0);
	draw_pixel(x+3 , y+10, 0);
	draw_pixel(x+7 , y+10, 0);
	draw_pixel(x+8 , y+10, 0);
	draw_pixel(x+9 , y+10, 0);
	draw_pixel(x+10, y+10, 0);
}

void draw_play(int x, int y){
	draw_pixel(x+1 , y  , 0);
	draw_pixel(x+2 , y  , 0);

	draw_pixel(x+0 , y+1, 0);
	draw_pixel(x+1 , y+1, 0);
	draw_pixel(x+2 , y+1, 0);
	draw_pixel(x+3 , y+1, 0);
	draw_pixel(x+4 , y+1, 0);

	draw_pixel(x+0 , y+2, 0);
	draw_pixel(x+1 , y+2, 0);
	draw_pixel(x+2 , y+2, 0);
	draw_pixel(x+3 , y+2, 0);
	draw_pixel(x+4 , y+2, 0);
	draw_pixel(x+5 , y+2, 0);

	draw_pixel(x+0 , y+3, 0);
	draw_pixel(x+1 , y+3, 0);
	draw_pixel(x+2 , y+3, 0);
	draw_pixel(x+3 , y+3, 0);
	draw_pixel(x+4 , y+3, 0);
	draw_pixel(x+5 , y+3, 0);
	draw_pixel(x+6 , y+3, 0);
	draw_pixel(x+7 , y+3, 0);

	draw_pixel(x+0 , y+4, 0);
	draw_pixel(x+1 , y+4, 0);
	draw_pixel(x+2 , y+4, 0);
	draw_pixel(x+3 , y+4, 0);
	draw_pixel(x+4 , y+4, 0);
	draw_pixel(x+5 , y+4, 0);
	draw_pixel(x+6 , y+4, 0);
	draw_pixel(x+7 , y+4, 0);
	draw_pixel(x+8 , y+4, 0);
	draw_pixel(x+9 , y+4, 0);

	draw_pixel(x+0 , y+5, 0);
	draw_pixel(x+1 , y+5, 0);
	draw_pixel(x+2 , y+5, 0);
	draw_pixel(x+3 , y+5, 0);
	draw_pixel(x+4 , y+5, 0);
	draw_pixel(x+5 , y+5, 0);
	draw_pixel(x+6 , y+5, 0);
	draw_pixel(x+7 , y+5, 0);
	draw_pixel(x+8 , y+5, 0);
	draw_pixel(x+9 , y+5, 0);
	draw_pixel(x+10, y+5, 0);

	draw_pixel(x+0 , y+6, 0);
	draw_pixel(x+1 , y+6, 0);
	draw_pixel(x+2 , y+6, 0);
	draw_pixel(x+3 , y+6, 0);
	draw_pixel(x+4 , y+6, 0);
	draw_pixel(x+5 , y+6, 0);
	draw_pixel(x+6 , y+6, 0);
	draw_pixel(x+7 , y+6, 0);
	draw_pixel(x+8 , y+6, 0);
	draw_pixel(x+9 , y+6, 0);

	draw_pixel(x+0 , y+7, 0);
	draw_pixel(x+1 , y+7, 0);
	draw_pixel(x+2 , y+7, 0);
	draw_pixel(x+3 , y+7, 0);
	draw_pixel(x+4 , y+7, 0);
	draw_pixel(x+5 , y+7, 0);
	draw_pixel(x+6 , y+7, 0);
	draw_pixel(x+7 , y+7, 0);

	draw_pixel(x+0 , y+8, 0);
	draw_pixel(x+1 , y+8, 0);
	draw_pixel(x+2 , y+8, 0);
	draw_pixel(x+3 , y+8, 0);
	draw_pixel(x+4 , y+8, 0);
	draw_pixel(x+5 , y+8, 0);

	draw_pixel(x+0 , y+9, 0);
	draw_pixel(x+1 , y+9, 0);
	draw_pixel(x+2 , y+9, 0);
	draw_pixel(x+3 , y+9, 0);
	draw_pixel(x+4 , y+9, 0);

	draw_pixel(x+1 , y+10, 0);
	draw_pixel(x+2 , y+10, 0);
}

	