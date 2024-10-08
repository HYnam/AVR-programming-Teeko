/*
 * project.c
 *
 * Main file
 *
 * Authors: Peter Sutton, Luke Kamols
 * Modified by Hiu Yi NAM 46604563
 */ 

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 16000000L
#include <util/delay.h>

#include "game.h"
#include "display.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

#define ESCAPE_CHAR 27

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete
	start_screen();
	
	// Loop forever,
	while(1) {
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void) {
	init_button_interrupts();
	// Setup serial port for 38400 baud communication with no echo
	// of incoming characters
	init_serial_stdio(38400,0);
	
	init_timer0();
	
	// Turn on global interrupts
	sei();
}

void start_screen(void) {
	/*======================================================
	2) Start Screen (Level 1 � 4 marks) 
	=======================================================*/
	
	// Clear terminal screen and output a message
	clear_terminal();
	move_terminal_cursor(10,10);
	printf_P(PSTR("Teeko"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("CSSE2010 project by Hiu Yi NAM 46604563"));
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	start_display();
	
	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1) {
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S') {
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED) {
			break;
		}
	}
}

void new_game(void) {
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the game and display
	initialise_game();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void) {
	
	uint32_t last_flash_time, current_time;
	uint8_t btn; //the button pushed
	
	char escape_sequence_char;
	uint8_t chars_in_escape_sequence = 0;
	
	last_flash_time = get_current_time();
	
	// We play the game until it's over
	while(!is_game_over()) {
		
		// We need to check if any button has been pushed, this will be
		// NO_BUTTON_PUSHED if no button has been pushed
		btn = button_pushed();
		
		char serial_input = 1;
		escape_sequence_char = -1;
		
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
			if (chars_in_escape_sequence == 0 && serial_input == ESCAPE_CHAR) {
				chars_in_escape_sequence ++;
				serial_input = -1;
			} else if (chars_in_escape_sequence == 1 && serial_input == '[') {
				chars_in_escape_sequence ++;
				serial_input = -1;
			} else if (chars_in_escape_sequence == 2) {
				escape_sequence_char = serial_input;
				serial_input = -1;
				chars_in_escape_sequence = 0;
			} else {
				chars_in_escape_sequence = 0;
			}
		}
		/*======================================================
		3) Move Cursor with Buttons (Level 1 � 12 marks)
		========================================================
		4) Move Cursor with Terminal Input (Level 1 � 5 marks) 
		=======================================================*/
		
		if (serial_input == 'w' || serial_input == 'W' || escape_sequence_char == 'A' || btn == BUTTON1_PUSHED) {
			// move the cursor upwards			
			move_display_cursor(0, 1);
			//flush the cursor
			last_flash_time-= 500;
		} else if (serial_input == 'a' || serial_input == 'A' || escape_sequence_char == 'D' || btn == BUTTON3_PUSHED) {
			// Move the cursor to the left			
			move_display_cursor(-1, 0);
			//flush the cursor
			last_flash_time-= 500;
		} else if (serial_input == 's' || serial_input == 'S' || escape_sequence_char == 'B' || btn == BUTTON0_PUSHED) {
			// Move the cursor downwards			
			move_display_cursor(0, -1);
			//flush the cursor
			last_flash_time-= 500;
		} else if (serial_input == 'd' || serial_input == 'D' || escape_sequence_char == 'C' || btn == BUTTON2_PUSHED) {
			// Move the cursor to the right			
			move_display_cursor(1, 0);
			//flush the cursor
			last_flash_time-= 500;
		}else if (serial_input == ' ') {
			//update the pieces (place, move, and pick handeling)
			update_piece();
		}

		current_time = get_current_time();
		if(current_time >= last_flash_time + 500) {
			// 500ms (0.5 second) has passed since the last time we
			// flashed the cursor, so flash the cursor
			flash_cursor();			
			// Update the most recent time the cursor was flashed
			last_flash_time = current_time;
		}
	}
	// We get here if the game is over.
}

void handle_game_over() {
	move_terminal_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press a button to start again"));
	
	while(button_pushed() == NO_BUTTON_PUSHED) {
		; // wait
	}
	
}
