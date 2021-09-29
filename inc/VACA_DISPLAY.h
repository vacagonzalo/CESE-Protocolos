#ifndef PROGRAMS_VACA_PROT_INC_VACA_DISPLAY_H_
#define PROGRAMS_VACA_PROT_INC_VACA_DISPLAY_H_

#include<stdint.h>

void displayInit();

void displayCursorPos( uint8_t x, uint8_t y );

void displayWrite( char const * str );

void displayClear();

#endif /* PROGRAMS_VACA_PROT_INC_VACA_DISPLAY_H_ */
