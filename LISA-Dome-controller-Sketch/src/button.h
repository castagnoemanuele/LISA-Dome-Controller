
#ifndef BUTTON_H	
#define BUTTON_H


#include <stdint.h> // Add missing include directive for uint8_t type
#include <WString.h>

struct Button {
	const uint8_t PIN;
	uint32_t numberKeyPresses;
	bool pressed; 
	String message;
};

#endif






