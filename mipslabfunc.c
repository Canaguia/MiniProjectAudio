/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

#define CANVAS_ARRAY_SIZE 512

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}


/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/   
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
    quicksleep(1000000);

    spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
    quicksleep(10);
	DISPLAY_DO_NOT_RESET;
    quicksleep(10);

    spi_send_recv(0x8D);
    spi_send_recv(0x14);

    spi_send_recv(0xD9);
    spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
    quicksleep(10000000);

    spi_send_recv(0xA1);
    spi_send_recv(0xC8);

    spi_send_recv(0xDA);
    spi_send_recv(0x20);
	
	spi_send_recv(0x20);
	spi_send_recv(0x0);

    spi_send_recv(0xAF);
	quicksleep(100);
	/* Change to data mode */
	DISPLAY_CHANGE_TO_DATA_MODE;
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 32; j++)
			spi_send_recv(~data[i*32 + j]);
	}
}

void display_update(void){
	int i;
	for(i = 0; i < CANVAS_ARRAY_SIZE; i++){
		spi_send_recv(canvas[i]);
	}
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}


/* OLED DISPLAY FUNCTIONS */
/* Function that handles drawing a canvas, used for plotting graphs */

// static uint8_t canvas[128 * 4] = { 0 }; // 128*32 (represents pixel grid as an array of unsigned 8-bit integers)

/* sets the pixel at the specified row and column to "ON" */
void draw_pixel(int x, int y, int colPix) {
	if(x < 32 && x >= 0 && y < 128 && y >= 0 ){
		int xOffset = x % 8;
		int page = (x + 8) / 8;
		int arrayPos = page * 128 - y;

		canvas[arrayPos] |= 1 << xOffset;
	}
	return;
}

void draw_string(uint8_t x, uint8_t y, char* str, char centered) {

	const char* i;
	uint8_t strLength = 0;
	if (centered == 1) {
		const char* a;
		for (a = str; *a != '\0'; a++) {
			strLength++;
		}
		if (strLength == 1) x = 14;
		else if (strLength == 2) x = 11;
		else if (strLength == 3) x = 8;
		else if (strLength == 4) x = 5;
		else x = 0;
	}

	for (i = str; *i != '\0'; i++) {
		char c = *i;
		/* Dont draw outside the screen */

		// if(c == 32) {
		//  x += 4;
		//  continue;
		// }

		int j;
		for (j = 0; j < 5; j++) {
			uint8_t data;
			if (c >= 65 && c <= 90) {
				data = charArray[(c - 65) * 5 + j];
			}
			else if (c >= 48 && c <= 58) {
				data = charArray[(c - 48 + 26) * 5 + j];
			}
			int k;
			for (k = 0; k < 8; k++) {
				if (data & 0x01) {
					draw_pixel(x + j, y + k, 0);
				}
				data = data >> 1;
			}
		}
		x += 7;
	}
}

void draw_sprite(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy, uint8_t data[]) {
	int i, j;
	for(i = 0; i < dx; i++){
		for(j = 0; j < dy; j++){
			if(data[j*dx + i]){
				draw_pixel(x + i, y + j, 0);
			}
		}
	}
}

/* display the current saved canvas where each bit represents a pixel on the screen */
void display_canvas(void) {
	// int i, j;

	// for (i = 0; i < 4; i++) {
	// 	DISPLAY_CHANGE_TO_COMMAND_MODE;

	// 	spi_send_recv(0x22); // Set page
	// 	spi_send_recv(i); // page num

	// 	//  Start at the left column
	// 	spi_send_recv(0x00); // set horiz 
	// 	spi_send_recv(0x10); // 1st column?

	// 	DISPLAY_CHANGE_TO_DATA_MODE;

	// 	for (j = 0; j < 128; j++)
	// 		spi_send_recv(canvas[i * 128 + j]);
	// }

	int i;
	for(i = 0; i < CANVAS_ARRAY_SIZE; i++){
		spi_send_recv(canvas[i]);
	}
}

/* TEMPORARY */
void clear_canvas() {
	int i;
	for (i = 0; i < CANVAS_ARRAY_SIZE; i++) {
		canvas[i] = 0;
	}
}

/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
