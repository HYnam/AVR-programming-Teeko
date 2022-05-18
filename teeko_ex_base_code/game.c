/*
 * game.c
 *
 * Contains functions relating to the play of the game Teeko
 *
 * Authors: Luke Kamols, Jarrod Bennett
 */

#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "display.h"

// Start pieces in the middle of the board
#define CURSOR_X_START ((int)(WIDTH/2))
#define CURSOR_Y_START ((int)(HEIGHT/2))

uint8_t board[WIDTH][HEIGHT];
// cursor coordinates should be /* SIGNED */ to allow left and down movement.
// All other positions should be unsigned as there are no negative coordinates.
int8_t cursor_x;
int8_t cursor_y;
uint8_t cursor_visible;
uint8_t current_player;

void initialise_game(void) {
	
	// initialise the display we are using
	initialise_display();
	
	// initialise the board to be all empty
	for (uint8_t x = 0; x < WIDTH; x++) {
		for (uint8_t y = 0; y < HEIGHT; y++) {
			board[x][y] = EMPTY_SQUARE;
		}
	}
		
	// set the starting player
	current_player = PLAYER_1;
	
	// also set where the cursor starts
	cursor_x = CURSOR_X_START;
	cursor_y = CURSOR_Y_START;
	cursor_visible = 0;
}

uint8_t get_piece_at(uint8_t x, uint8_t y) {
	// check the bounds, anything outside the bounds
	// will be considered empty
	if (x < 0 || x >= WIDTH || y < 0 || y >= WIDTH) {
		return EMPTY_SQUARE;
	} else {
		//if in the bounds, just index into the array
		return board[x][y];
	}
}

void flash_cursor(void) {
	
	if (cursor_visible) {
		// we need to flash the cursor off, it should be replaced by
		// the colour of the piece which is at that location
		uint8_t piece_at_cursor = get_piece_at(cursor_x, cursor_y);
		update_square_colour(cursor_x, cursor_y, piece_at_cursor);
		
	} else {
		// we need to flash the cursor on
		update_square_colour(cursor_x, cursor_y, CURSOR);
	}
	cursor_visible = 1 - cursor_visible; //alternate between 0 and 1
}

//check the header file game.h for a description of what this function should do
// (it may contain some hints as to how to move the pieces)
void move_display_cursor(int8_t dx, int8_t dy) {
	//YOUR CODE HERE
	/*suggestions for implementation:
	 * 1: remove the display of the cursor at the current location
	 *		(and replace it with whatever piece is at that location)
	 * 2: update the positional knowledge of the cursor, this will include
	 *		variables cursor_x, cursor_y and cursor_visible. Make sure you
	 *		consider what should happen if the cursor moves off the board.
	 * 3: display the cursor at the new location
	 * 4: reset the cursor flashing cycle. See project.c for how the cursor
	 *		is flashed.
	 */
	
}

uint8_t is_game_over(void) {
	// YOUR CODE HERE
	// Detect if the game is over i.e. if a player has won.
	return 0;
}