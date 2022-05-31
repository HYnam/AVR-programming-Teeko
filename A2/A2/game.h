/*
** game.h
**
** Written by Luke Kamols
**
** Function prototypes for those functions available externally
*/

#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>

// initialise the display of the board, this creates the internal board
// and also updates the display of the board
void initialise_game(void);


// returns which piece is located at position (x,y)
// the value returned will be SQUARE_EMPTY, SQUARE_P1 or SQUARE_P2
// anything outside the bounds of the boards will be SQUARE_EMPTY
uint8_t get_piece_at(uint8_t x, uint8_t y);

// update the cursor display, by changing whether it is visible or not
// call this function at regular intervals to have the cursor flash
void flash_cursor(void);

// moves the position of the cursor by (dx, dy) such that if the cursor
// started at (cursor_x, cursor_y) then after this function is called, 
// it should end at ( (cursor_x + dx) % WIDTH, (cursor_y + dy) % HEIGHT)
// the cursor should be displayed after it is moved as well
void move_display_cursor(int8_t dx, int8_t dy);

// returns 1 if the game is over, 0 otherwise
uint8_t is_game_over(void);

//place move and pick pieces
void update_piece( void );

//draw the pieces in the game board
void draw_game( void );

// display longest line
void print_longest_line( void );

#endif /* GAME_H_ */