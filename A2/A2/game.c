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
#include "terminalio.h"

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
/********************************/
//===
uint8_t player_1_piece_count; // 0 --4 pieces when player 1 and 2 reach 4 pieces start game phase2
uint8_t player_2_piece_count; // 0 --4 pieces when player 1 and 2 reach 4 pieces start game phase2
uint8_t player_1_pos[4]; //store location of pieces
uint8_t player_2_pos[4]; //store location of pieces

//===
uint8_t piece_is_pickedup; //bool to test if the piece is pickedUp by the cursor to move
int8_t player_1_piece_pos;//this variables in range [1..4] to detect which piece we are holding for player 1
int8_t player_2_piece_pos;//this variables in range [1..4] to detect which piece we are holding for player 2
int8_t cursor_x_old; // this for deleting player1 or player 2 and replace it with EMPTY_SQUARE after moving
int8_t cursor_y_old; // this for deleting player1 or player 2 and replace it with EMPTY_SQUARE after moving

// all possibles wins : TODO: delete square format win, and decrement POS_WINS
#define POS_WINS 44
int pos_win[POS_WINS][4] = {
	{0, 1, 2, 3},   {1,2,3,4} ,  {5,6,7,8} ,    {6,7,8,9}, {10,11,12,13},
	{11,12,13,14}, {15,16,17,18}, {16,17,18,19}, {20,21,22,23},{21,22,23,24},
	{0,5,10,15}  ,  {5,10,15,20},   {1,6,11,16}, {6,11,16,21} ,{2,7,12,17},
	{7,12,17,22},{3,8,13,18},{8,13,18,23},{4,9,14,19},{9,14,19,24},
	{0,6,12,19},{6,12,18,24},{4,8,12,16},{8,12,16,20},{5,11,17,23},
	{1,7,13,19},{3,7,11,15},{21,17,13,9},{0,1,5,6},{1,2,6,7},
	{2,3,7,8}, {3,4,8,9}, {5,6,10,11}, {6,7,11,12} , {7,8,12,13},
	{8,9,13,14}, {10,11,15,16}, {11,12,16,17}, {12,13,17,18} , {13,14,18,19},
	{15,16,20,21}, {16,17,21,22},{17,18,22,23},{18,19,23,24}
	
};
/******************************/






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
	//
	player_1_piece_count = 0;
	player_2_piece_count = 0;
	for(int i =0; i<4; i++) {
		player_2_pos[i] = player_1_pos[i] = 0;
	}		
	piece_is_pickedup = 0; // false
}


uint8_t get_piece_row(void) {
	return cursor_x;
}

uint8_t get_piece_column(void) {
	return cursor_y;
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

/***********************************************************/

void move_display_cursor(int8_t dx, int8_t dy) {
	//test if the cursor is holding a piece
	if(piece_is_pickedup) {
		/*** GAME PHASE 2 ***/
		//when holding piece, the piece doesn't wrap around the board
		int tx = cursor_x + dx - cursor_x_old;
		int ty = cursor_y + dy - cursor_y_old;
		if( (cursor_x + dx < WIDTH) && (tx <= 1 && tx >= -1) ){
			cursor_x = cursor_x + dx;
		}
		
		if( (cursor_y + dy < HEIGHT) && (ty <= 1 && ty >= -1) ){
			cursor_y = cursor_y + dy;
		}
		
	}else {
		//Move Cursor normaly, wrap arount the board
		cursor_x = (cursor_x + dx) % WIDTH;
		if(cursor_x<0) cursor_x = WIDTH - 1;
		cursor_y = (cursor_y + dy) % HEIGHT;
		if(cursor_y<0) cursor_y = HEIGHT - 1;

	}
	
	initialise_display();
	draw_game();
}

uint8_t is_game_over(void) {
	if(player_1_piece_count < 4 || player_2_piece_count< 4) return 0;
	
	for (int m = 0; m < POS_WINS; m++) {
		int count_win = 0;
		
		if(current_player == PLAYER_2) {
			//test if the previous player(player1) has win the game
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (player_1_pos[i] == pos_win[m][j]) {
						count_win++;
						continue;
					}
				}
			}
			if (count_win == 4){
				move_terminal_cursor(0, 0);
				printf("player 1  win");
				return PLAYER_1;
			}
			
			}else if(current_player == PLAYER_1){
			//test if the previous player(player2) has win the game
			count_win = 0;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (player_2_pos[i] == pos_win[m][j]) {
						count_win++;
						continue;
					}
				}
			}
			if (count_win == 4){
				move_terminal_cursor(0, 0);
				printf("player 2 win");
				return PLAYER_2;
			}
			
		}
		
	}
	return 0;
}
void update_piece( void ) {
    
    // ***** Game Phase 1 ***** //
    // ends when all 8 pieces have been placed on the board
    
	if(player_1_piece_count < 4 || player_2_piece_count< 4) {
    	/* place a piece */
    	// test for legal move. is piece on top of another piece
    	uint8_t piece_at_cursor = get_piece_at(cursor_x, cursor_y);
    	
    	if(piece_at_cursor == EMPTY_SQUARE) {
    		board[cursor_x][cursor_y] = current_player;
    
    		//***Turn Indicator***
    		update_square_colour(cursor_x, cursor_y, current_player);
    		int pos = cursor_y * WIDTH + cursor_x;
    		    		
    		if(current_player == PLAYER_1) {
    		    player_1_pos[player_1_piece_count] = pos;
    		    player_1_piece_count++;
    		    
    	        set_display_attribute(FG_RED);
    	        move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y - 1);
    			printf("Current player: 2 (red)  ");
    		}else if(current_player == PLAYER_2){
    		    
    		    player_2_pos[player_2_piece_count] = pos;
    		    player_2_piece_count++;
    		    
    			set_display_attribute(FG_GREEN);
    			move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y - 1);
    			printf("Current player: 1 (green)");
    		}
    		// switch player
    		current_player = 3 - current_player; //alternate between 1 and 2
    	}	    
	}else {
	    // ***** Game Phase 2 ***** //
        
	    /* release the piece */
	    //place it again with space bar
	    if(piece_is_pickedup) {
	        
	        //is it the same location
	        if(cursor_y == cursor_y_old && cursor_x == cursor_x_old) {
	            return;
	        }
	        //is it local location, 
	        uint8_t piece_at_cursor = get_piece_at(cursor_x, cursor_y);
	        if(piece_at_cursor != EMPTY_SQUARE) {
	            return;
	        }
	        
	        int pos = cursor_y*WIDTH + cursor_x;
	        if(current_player == PLAYER_1) {
	           	player_1_pos[player_1_piece_pos] = pos;
	            board[ cursor_x_old ][ cursor_y_old ] = EMPTY_SQUARE;
	            board[ cursor_x ][ cursor_y ] = PLAYER_1;
	            
	            set_display_attribute(FG_RED);
    	        move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y - 1);
    			printf("Current player: 2 (red)  ");
	        }else if (current_player == PLAYER_2){
	            player_2_pos[player_2_piece_pos] = pos;
	            board[ cursor_x_old ][ cursor_y_old ] = EMPTY_SQUARE;
	            board[ cursor_x ][ cursor_y ] = PLAYER_2;
	            
	            set_display_attribute(FG_GREEN);
    	        move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y - 1);
    			printf("Current player: 1 (green)");
	        }
	        piece_is_pickedup = 0;
	        // switch player
    		current_player = 3 - current_player; //alternate between 1 and 2
    		
    	/* pick a piece */
	    }else {
	        
    	    uint8_t piece_at_cursor = get_piece_at(cursor_x, cursor_y);
    	    int pos = cursor_y * WIDTH + cursor_x;
    	    
    	    if(piece_at_cursor == PLAYER_1 && current_player == PLAYER_1) {
    	        set_display_attribute(FG_GREEN);
    	        move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y - 1);
    			printf("Current player: 1 (green)");
    	        
    	        piece_is_pickedup = 1;
    	        cursor_x_old = cursor_x;
    	        cursor_y_old = cursor_y;
    	        for(int j= 0; j<4; j++) {
    	            if( player_1_pos[j] == pos){
    	                player_1_piece_pos = j;
    	                break;
    	            }
    	        }
    	    }else if (piece_at_cursor == PLAYER_2 && current_player == PLAYER_2) {
    	        set_display_attribute(FG_RED);
    	        move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y - 1);
    			printf("Current player: 2 (red)  ");
    			
    	        piece_is_pickedup = 1;
    	        cursor_x_old = cursor_x;
    	        cursor_y_old = cursor_y;
    	        for(int j= 0; j<4; j++) {
    	            if( player_2_pos[j] == pos){
    	                player_2_piece_pos = j;
    	                break;
    	            }
    	        }
    	    }else {
    	        //do nothing: TODO
    	    }
    	    
	    }//else:piece not picked_up
	}//else:there are other piece not placed
	
}
void draw_game( void ) {
	for (uint8_t x = 0; x < WIDTH; x++) {
		for (uint8_t y = 0; y < HEIGHT; y++) {
			update_square_colour(x, y, board[x][y]);
		}
	}
}
