#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#define MAX_OBS 10

uint8_t obsPos[10][2] = {50,0,50,0,50,0,50,0,50,0,50,0,50,0,50,0,50,0,50,0}; // spawn them out of view
uint8_t obsSpawnCtr = 0;
int obsSpawnCooldown = 100;
int obsSpawnCooldownCtr = 100;

/* Spawns a new entity at a random position */
void spawn_entity() {
	int x;
	obsSpawnCooldown = obsSpawnCooldown - obsSpawnCooldown / 4;
	obsSpawnCooldownCtr = obsSpawnCooldown;

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

	for (i = 0; i <= 10; i++) {
		obsPos[i][1]++; // scroll by incr. y-level
	}

	obsSpawnCooldownCtr--;
	if (obsSpawnCooldownCtr <= 0) {
		spawn_entity();
	}
	return;
}

