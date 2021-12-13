/*gameplay.c
* original code by Filip Karlsson
* with functions from Axel Isaksson
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#define EEPROM_ADDR 0x50
#define totalHighscores 25

uint16_t memAddress = 0x0000; // accepts addresses up to 3FFF


/* Helper functions provided by hello-temperature example project: */

/* Wait for I2C bus to become idle */
void i2c_idle() {
	while (I2C1CON & 0x1F || I2C1STAT & (1 << 14)); //TRSTAT - return if idle OR if Master transmit is not in progress
}

/* Send one byte on I2C bus, return ack/nack status of transaction */
uint8_t i2c_send(uint8_t data) {
	i2c_idle();
	I2C1TRN = data; // 8-bit data-out register
	i2c_idle();
	return !(I2C1STAT & (1 << 15)); //ACKSTAT (0= ACK, 1= NACK)
}

/* Receive one byte from I2C bus */
uint8_t i2c_recv() {
	i2c_idle();
	I2C1CONSET = 1 << 3; //RCEN = 1 (recieve-enable bit)
	i2c_idle();
	I2C1STATCLR = 1 << 6; //I2COV = 0 (recieve-overflow flag -> 1 if  I2C1RCV register is still holding the previous byte)
	return I2C1RCV;
}

/* Send acknowledge conditon on the bus */
void i2c_ack() {
	i2c_idle();
	I2C1CONCLR = 1 << 5; //ACKDT = 0 (ack-status)
	I2C1CONSET = 1 << 4; //ACKEN = 1 (send info)
}

/* Send not-acknowledge conditon on the bus */
void i2c_nack() {
	i2c_idle();
	I2C1CONSET = 1 << 5; //ACKDT = 1 (nack-status)
	I2C1CONSET = 1 << 4; //ACKEN = 1 (send info)
}

/* Send start conditon on the bus */
void i2c_start() {
	i2c_idle();
	I2C1CONSET = 1 << 0; //SEN (start condition enable)
	i2c_idle();
}

/* Send restart conditon on the bus */
void i2c_restart() {
	i2c_idle();
	I2C1CONSET = 1 << 1; //RSEN (repeated start condition enable)
	i2c_idle();
}

/* Send stop conditon on the bus */
void i2c_stop() {
	i2c_idle();
	I2C1CONSET = 1 << 2; //PEN (stop condition enable)
	i2c_idle();
}



/* Original code starts here: */

// Reads 8 bytes sequentially from given start-address and returns them (AAA DDD EE)	
void read_highscore(uint16_t address, uint8_t bytes, uint8_t* data) {
	int i;
	uint8_t t;

	do {
		i2c_start();
	} while (!i2c_send((EEPROM_ADDR << 1)));

	uint8_t addrMSB = ((address & 0xff00) >> 8);
	uint8_t addrLSB = address & 0x00ff;
	i2c_send(addrMSB); // Send high byte address to be accessed
	i2c_send(addrLSB); // Send low byte address to be accessed

	/* repeat start */
	do {
		i2c_start();
	} while (!i2c_send((EEPROM_ADDR << 1) | 1));

	// start recieving data
	for (i = 0; i < bytes; i++) {
		t = i2c_recv(); //recieves byte
		i2c_ack(); // ACK
		*data = t;
		data++;
	}

	// stop reading with NACK+STOP
	i2c_nack();
	i2c_stop();

	return;
}

// write up to 64 bytes sequentially to a given start-address
void write_highscore(uint16_t address, uint8_t bytes, uint8_t data[]) {
	int i;

	/* Send start condition and address of the EEPROM with write flag (lowest bit = 0)
		until the temperature sensor sends ACK */
	do {
		i2c_start();
	} while (!i2c_send(EEPROM_ADDR << 1)); //this shift sends a 0 in LSB

	uint8_t addrMSB = ((address & 0xff00) >> 8);
	uint8_t addrLSB = address & 0x00ff;
	i2c_send(addrMSB); // Send high byte address to be accessed
	i2c_send(addrLSB); // Send low byte address to be accessed

	// send designated amount of bytes to address location (loaded sequentially)	
	for (i = 0; i < bytes; i++) {
		i2c_send(data[i]);
	}

	// Send stop condition 
	i2c_stop();

	return;
}

/* Clears all saved highscores at memAddress*/
void clear_highscore_data() {
	uint8_t temp[25] = { 0 }; // clears entire page on write of more than 1-byte (64-bytes)
	write_highscore(memAddress, 25, temp);
	return;
}

/* checks if a given highscore is in the top 5 and then loads it into non-volatile EEPROM memory
	returns 1 if highscore got put into top 5, returns 0 if not*/
char insert_highscore(char playerName[], int playerHighscore) {
	int i;
	int highscore = 0;

	// read highscores from EEPROM storage and put them into 2D-array
	char charArr[6 * 3];
	int scoreArr[6];
	uint8_t tempArr[totalHighscores];
	read_highscore(memAddress, totalHighscores, tempArr);
	for (i = 0; i < 5; i++) {
		//store 3-char as string
		charArr[i * 3] = (char)tempArr[i * 5];
		charArr[i * 3 + 1] = (char)tempArr[i * 5 + 1];
		charArr[i * 3 + 2] = (char)tempArr[i * 5 + 2];

		//store MSB & LSB as highscore;
		highscore = (tempArr[i * 5 + 3] << 8);
		highscore |= tempArr[i * 5 + 4];
		scoreArr[i] = highscore;
	}

	// find and shift appropriate position for playerhighscore
	int r = 5;
	for (i = 4; i >= 0; i--) {
		if (scoreArr[i] < playerHighscore) {
			scoreArr[i + 1] = scoreArr[i];
			charArr[(i + 1) * 3] = charArr[i * 3];
			charArr[(i + 1) * 3 + 1] = charArr[i * 3 + 1];
			charArr[(i + 1) * 3 + 2] = charArr[i * 3 + 2];

			r = i;
		}
	}

	// load players name into highscore list
	scoreArr[r] = playerHighscore;
	charArr[r * 3] = playerName[0];
	charArr[r * 3 + 1] = playerName[1];
	charArr[r * 3 + 2] = playerName[2];

	// write top 5 into EEPROM storage
	for (i = 0; i < 5; i++) {
		//load 3-char string
		tempArr[i * 5] = (uint8_t)charArr[i * 3];
		tempArr[i * 5 + 1] = (uint8_t)charArr[i * 3 + 1];
		tempArr[i * 5 + 2] = (uint8_t)charArr[i * 3 + 2];

		//load MSB and LSB of highscore
		tempArr[i * 5 + 3] = (uint8_t)((scoreArr[i] & 0xff00) >> 8);
		tempArr[i * 5 + 4] = (uint8_t)(scoreArr[i] & 0xff);
	}
	write_highscore(memAddress, totalHighscores, tempArr);

	if (r >= 5) {
		return 0;
	}
	return 1;
}

/* load and display all highscores (top 5) */
void display_highscores() {
	int i;
	int highscore = 0;

	// read highscores from EEPROM storage and put them into 2D-array
	char charArr[6 * 3];
	int scoreArr[6];
	uint8_t tempArr[totalHighscores];
	read_highscore(memAddress, totalHighscores, tempArr);
	for (i = 0; i < 5; i++) {
		// store 3-char as string
		charArr[i * 3] = (char)tempArr[i * 5];
		charArr[i * 3 + 1] = (char)tempArr[i * 5 + 1];
		charArr[i * 3 + 2] = (char)tempArr[i * 5 + 2];

		// store MSB & LSB as highscore;
		highscore = (tempArr[i * 5 + 3] << 8);
		highscore |= tempArr[i * 5 + 4];
		scoreArr[i] = highscore;
	}

	// use itoa to convert top 5 to decimal string && print
	char name[] = "   ";
	for (i = 0; i < 5; i++) {
		name[0] = charArr[i * 3];
		name[1] = charArr[i * 3 + 1];
		name[2] = charArr[i * 3 + 2];
		draw_string(8, 16 + 22*i, name,1);
		draw_string(10, 24 + 22*i, itoaconv(scoreArr[i]),1);
	}

	return;
}