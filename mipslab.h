/* Header.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson
*/

/* Declare display-related functions from Render.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);

uint8_t spi_send_recv(uint8_t data);

// NEW
uint8_t canvas[512];
void draw_pixel(int x, int y, int colPix);
void draw_sprite(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy, uint8_t *data);
void draw_string(uint8_t x, uint8_t y, char *str);
void draw_int(uint8_t x, uint8_t y, int num);
void display_canvas(void);
void clear_canvas(void);

extern uint8_t state;

//Gameplay
extern uint8_t currentY;
extern uint8_t currentX;
extern uint8_t playerInv;
extern uint8_t playerInvCtr;
extern uint8_t playerLives;

void playerMovement(void);
void playerMoveUp(void);
void playerMoveDown(void);
void playerMoveLeft(void);
void playerMoveRight(void);

void player_out_of_bounds(void);

//Walls
void generate_walls(void);

//Game-Sprites
void draw_balloon3(int x, int y);
void draw_balloon2(int x, int y);
void draw_balloon1(int x, int y);
void draw_balloon0(int x, int y);
void draw_bird(int x, int y);

//UI-Sprites
void draw_pause(int x, int y);
void draw_play(int x, int y);

/* Declare lab-related functions from Render.c */
char * itoaconv( int num );
void gameRunning(void);
void quicksleep(int cyc);

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

extern const uint8_t const icon2[128];
extern const uint8_t const balloon[128];
extern const uint8_t const charArray[185];


/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );

/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);