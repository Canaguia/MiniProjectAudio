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

// Canvas
uint8_t canvas[512];
void draw_pixel(int x, int y, int colPix);
void draw_sprite(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy, uint8_t data[]);
void draw_string(uint8_t x, uint8_t y, char *str, char centered);
void sprite_anim(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy, int ctr, uint8_t frame_nr, uint8_t* sprite_array);
void draw_int(uint8_t x, uint8_t y, int num);
void display_canvas(void);
void clear_canvas(void);

extern uint8_t state;
extern int score;

// buttons
#define BTN1_PRESSED (getbtns() & 1)
#define BTN2_PRESSED (getbtns() & 2)
#define BTN3_PRESSED (getbtns() & 4)
#define BTN4_PRESSED (getbtns() & 8)

#define SW1_SWITCHED (getsw() & 1)
#define SW2_SWITCHED (getsw() & 2)
#define SW3_SWITCHED (getsw() & 4)
#define SW4_SWITCHED (getsw() & 8)


//Gameplay
extern uint8_t currentY;
extern uint8_t currentX;
extern uint8_t playerInv;
extern uint8_t playerInvCtr;
extern uint8_t playerLives;
extern int analogBaseline;

void playerMovement(void);
void playerMoveUp(void);
void playerMoveDown(void);
void playerMoveLeft(void);
void playerMoveRight(void);

void player_out_of_bounds(void);
int sample_average(void);

//Walls
void generate_walls(void);

//Entities
void entity_scroll(void);
void render_entity(void);
void check_entity_collision(void);
void reset_entity_position(void);

//Game-Sprites
void draw_balloon3(int x, int y);
void draw_balloon2(int x, int y);
void draw_balloon1(int x, int y);
void draw_balloon0(int x, int y);
void draw_bird(int x, int y);

//UI-Sprites
void draw_pause(int x, int y);
void draw_play(int x, int y);

/* Declare lab-related functions from Mipslabfunc */
char * itoaconv( int num );
void gameRunning(void);
void quicksleep(int cyc);
uint8_t pseudo_random(int max);

// Highscore
void display_highscores();
char insert_highscore(char playerName[], int playerHighscore);
void clear_highscore_data(void);

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
void setled(uint8_t numLed);
void clearled(uint8_t numLed);


// SPRITE
uint8_t const w;
uint8_t const h;

uint8_t uphi_art[2][4096];
uint8_t shruiken[3][81];