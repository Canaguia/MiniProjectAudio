
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
   /* Declare a helper function which is local to this file */
static void num32asc(char* s, int);

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

#define CANVAS_ARRAY_SIZE 512


uint8_t spi_send_recv(uint8_t data) {
	while (!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while (!(SPI2STAT & 1));
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

void draw_pixel(int x, int y, int colPix) {
	if (x < 32 && x >= 0 && y < 128 && y >= 0) {
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
	for (i = 0; i < dx; i++) {
		for (j = 0; j < dy; j++) {
			if (data[j * dx + i]) {
				draw_pixel(x + i, y + j, 0);
			}
		}
	}
}

void sprite_anim(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy, int ctr, uint8_t frame_nr, uint8_t* sprite_array) {
	int i;
	for (i = 0; i < frame_nr; i++) {
		if (ctr % frame_nr == i) {
			draw_sprite(x, y, dx, dy, sprite_array + dx * dy * i);
		}
	}
}

void display_update(void) {
	int i;
	for (i = 0; i < CANVAS_ARRAY_SIZE; i++) {
		spi_send_recv(canvas[i]);
	}
}

void clear_canvas() {
	int i;
	for (i = 0; i < CANVAS_ARRAY_SIZE; i++) {
		canvas[i] = 0;
	}
}