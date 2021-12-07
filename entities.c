#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#define MAX_OBS 10
#define DEF_OBS_COOLDOWN 100
#define OBS_H 9
#define OBS_W 9

#define playerWidth 5
#define playerHeight 7

uint8_t obsPos[10][2] = {50,0,50,0,50,0,50,0,50,0,50,0,50,0,50,0,50,0,50,0}; // spawn them out of view
uint8_t obsSpawnCtr = 0;
uint8_t obsSpawnCooldown = DEF_OBS_COOLDOWN;
int obsSpawnCooldownCtr = DEF_OBS_COOLDOWN;

/* Spawns a new entity at a random position */
void spawn_entity() {
	int x;

	x = pseudo_random(4);
	obsPos[obsSpawnCtr][0] = 8 * x; // x	
	obsPos[obsSpawnCtr][1] = 0; // y

	obsSpawnCtr++;
	if (obsSpawnCtr == MAX_OBS) {
		obsSpawnCtr = 0;
	}
	return;
}

/* Render all entities */
void render_entity() {
	int i;

	for (i = 0; i <= MAX_OBS; i++) {
		draw_bird(obsPos[i][0], obsPos[i][1]); // change to render sometin
	}

	return;
}

/* Scroll all entities */
void entity_scroll() {
	int i;
	draw_string(0, 20, itoaconv(obsSpawnCooldownCtr), 1);
	draw_string(0, 30, itoaconv(obsSpawnCooldown), 1);

	for (i = 0; i <= MAX_OBS; i++) {
		if (obsPos[i][1] < 130) {
			obsPos[i][1]++; // scroll by incr. y-level
		}
	}

	
	obsSpawnCooldownCtr--;

	if (obsSpawnCooldownCtr <= 0) {
		if (obsSpawnCooldown > 20) {
			obsSpawnCooldown--;
		}
		obsSpawnCooldownCtr = obsSpawnCooldown;
		obsSpawnCooldownCtr += pseudo_random(10);
		spawn_entity();
	}
	return;
}

/* check if two boxes collide */
char check_collision_box(int x, int y, int w, int h, int px, int py, int pw, int ph) {

	if (x < px + pw &&
		x + w > px &&
		y < py + ph &&
		y + h > py)
	{
		return 1;
	}
	return 0;
}

/* Check if two circles collide */
char check_collision_circle(int x, int y, int r, int px, int py, int pr) {
	int max = r + pr;

	if ((x - px) * (x - px) + (y - py) * (y - py) < (max * max))
	{
		return 1;
	}
	return 0;
}

/* check if an entity has collided with the player */
void check_entity_collision() {
	int i;

	for (i = 0; i <= 10; i++) {
		if (check_collision_box(obsPos[i][0], obsPos[i][1], OBS_W, OBS_H, currentX, currentY, playerWidth, playerHeight)) {
			if (!playerInv) {
				player_out_of_bounds();
			}
		}
	}
	return;
}

/* reset position of all entities */
void reset_entity_position() {
	int i;

	for (i = 0; i < MAX_OBS*2; i += 2)
	{
		obsPos[i][0] = 50;
		obsPos[i][1] = 0;
	}
	return;
}



