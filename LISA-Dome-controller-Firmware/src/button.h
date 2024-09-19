/**
 * @file button.h
 * @brief Header file for the Button structure.
 *
 * This file contains the definition of the Button structure, which represents
 * a button with a specific pin, number of key presses, pressed state, and a message.
 */

#ifndef BUTTON_H	
#define BUTTON_H

#include <stdint.h> // Add missing include directive for uint8_t type
#include <WString.h>

struct Button {
	const uint8_t PIN; /// pin of the microcontroller connected to the button
	uint32_t numberKeyPresses; ///unused counter 
	bool pressed; ///actual status of the button
	String message; ///message that gets displayed on the oled screen 
};

#endif






