/* mipslabmain.c

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall

   Latest update 2017-04-21 by F Lundevall

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

uint8_t state = 0;

int main(void) {
    /*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready *///(PBDIV = PBCLOCK DIVISOR)
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */
	
	/* Set up output pins (LEDS */
	AD1PCFG = 0xFFFF; // disable all analog input pins
	ODCE = 0x0; // open drain off (variable output voltage)
	TRISECLR = 0xFF; // set E-pins as output
	PORTE = 0x0; // clear ports
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */ //RF1, RD5, RD6, RD7 (pushbuttons)
	/*
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	*/
	TRISDSET = (7 << 5);
	TRISDSET = (0xF<<8);
	TRISFSET = (1 << 1);

	/* Set up input pins */ //RD8, RD9, RD10, RD11 (switches)

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;

	display_init();	
	labinit(); /* Do any lab-specific initialization */

	while(1){
		masterGameLoop();
	}
	return 0;
}
