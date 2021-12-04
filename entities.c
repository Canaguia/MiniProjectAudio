#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#define MAX_OBS 10
#define DEF_OBS_COOLDOWN 50

uint8_t obsPos[10][2] = {50,0,50,0,50,0,50,0,50,0,50,0,50,0,50,0,50,0,50,0}; // spawn them out of view
uint8_t obsSpawnCtr = 0;
uint8_t obsSpawnCooldown = DEF_OBS_COOLDOWN;
int obsSpawnCooldownCtr = 50;

/* Spawns a new entity at a random position */
void spawn_entity() {
	int x;

	x = pseudo_random(3);
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

	for (i = 0; i <= 10; i++) {
		draw_bird(obsPos[i][0], obsPos[i][1]); // change to render sometin
	}

	return;
}

/* Scroll all entities */
void entity_scroll() {
	int i;
	draw_string(0, 20, itoaconv(obsSpawnCooldownCtr), 1);
	draw_string(0, 30, itoaconv(obsSpawnCooldown), 1);

	for (i = 0; i <= 10; i++) {
		obsPos[i][1]++; // scroll by incr. y-level
	}

	obsSpawnCooldownCtr--;
	if (obsSpawnCooldownCtr <= 0) {
		obsSpawnCooldown--;
		obsSpawnCooldownCtr = obsSpawnCooldown;
		spawn_entity();
	}
	return;
}

