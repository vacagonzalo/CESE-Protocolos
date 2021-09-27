#include "VACA_DISPLAY.h"
#include "portI2C.h"
#include <stdbool.h>

#define CLEAR_DISPLAY  0b00000001
#define ENTRY_MODE_SET 0b00000100
#define CONTROL        0b00001000
#define FUNCTION_SET   0b00100000
#define SET_DDRAM_ADDR 0b10000000

#define ENTRY_MODE_SET_INCREMENT 0b00000010
#define ENTRY_MODE_SET_DECREMENT 0b00000000
#define ENTRY_MODE_SET_SHIFT     0b00000001
#define ENTRY_MODE_SET_NO_SHIFT  0b00000000

#define CONTROL_DISPLAY_ON  0b00000100
#define CONTROL_DISPLAY_OFF 0b00000000
#define CONTROL_CURSOR_ON   0b00000010
#define CONTROL_CURSOR_OFF  0b00000000
#define CONTROL_BLINK_ON    0b00000001
#define CONTROL_BLINK_OFF   0b00000000

#define FUNCTION_SET_8BITS    0b00010000
#define FUNCTION_SET_4BITS    0b00000000
#define FUNCTION_SET_2LINES   0b00001000
#define FUNCTION_SET_1LINE    0b00000000
#define FUNCTION_SET_5x10DOTS 0b00000100
#define FUNCTION_SET_5x8DOTS  0b00000000

#define LINE1_FIRST_CHARACTER_ADDRESS 0
#define LINE2_FIRST_CHARACTER_ADDRESS 64
#define LINE3_FIRST_CHARACTER_ADDRESS 20
#define LINE4_FIRST_CHARACTER_ADDRESS 84

#define RS_INSTRUCTION 0
#define RS_DATA 1

#define RW_WRITE 0
#define RW_READ 1

#define I2C_ADDRESS 78
#define EXPANDER_DATA_SIZE 1

typedef enum {
	RS, RW, EN, A, D4, D5, D6, D7
} i2cExpanderPin_t;

static uint8_t dataExpander = 0x00;

void writeExpanderPin(i2cExpanderPin_t pin, bool value);
void writeDisplayByCode(bool code, uint8_t data);
void writeDisplayData(uint8_t data);

void displayInit()
{
	portI2Cinit();

	writeExpanderPin(A, 1);
	portDelay(50);

	// Envío el mismo msg varias veces para verificar el funcionamiento
	// con el analizador lógico.
	writeDisplayByCode(RS_INSTRUCTION, FUNCTION_SET
			| FUNCTION_SET_4BITS);
	portDelay(1);
	writeDisplayByCode(RS_INSTRUCTION, FUNCTION_SET
			| FUNCTION_SET_4BITS);
	portDelay(1);
	writeDisplayByCode(RS_INSTRUCTION, FUNCTION_SET
			| FUNCTION_SET_4BITS);
	portDelay(1);
	writeDisplayByCode(RS_INSTRUCTION, FUNCTION_SET
			| FUNCTION_SET_4BITS);
	portDelay(1);

	writeDisplayByCode(RS_INSTRUCTION, FUNCTION_SET
			| FUNCTION_SET_4BITS
			| FUNCTION_SET_2LINES
			| FUNCTION_SET_5x8DOTS);
	portDelay(1);

	writeDisplayByCode(RS_INSTRUCTION, CONTROL
			| CONTROL_DISPLAY_OFF
			| CONTROL_CURSOR_OFF
			| CONTROL_BLINK_OFF);
	portDelay(1);

	writeDisplayByCode(RS_INSTRUCTION, CLEAR_DISPLAY);
	portDelay(1);

	writeDisplayByCode(RS_INSTRUCTION, ENTRY_MODE_SET
			| ENTRY_MODE_SET_INCREMENT
			| ENTRY_MODE_SET_NO_SHIFT);
	portDelay(1);

	writeDisplayByCode(RS_INSTRUCTION, CONTROL
			| CONTROL_DISPLAY_ON
			| CONTROL_CURSOR_OFF
			| CONTROL_BLINK_OFF);
	portDelay(1);
}

void displayCursorPos( uint8_t x, uint8_t y )
{
	switch(y)
	{
	case 0:
		writeDisplayByCode(RS_INSTRUCTION, SET_DDRAM_ADDR
				| (LINE1_FIRST_CHARACTER_ADDRESS + x));
		portDelay(1);
		break;
	case 1:
		writeDisplayByCode(RS_INSTRUCTION, SET_DDRAM_ADDR
				| (LINE2_FIRST_CHARACTER_ADDRESS + x));
		portDelay(1);
		break;
	case 2:
		writeDisplayByCode(RS_INSTRUCTION, SET_DDRAM_ADDR
				| (LINE3_FIRST_CHARACTER_ADDRESS + x));
		portDelay(1);
		break;
	case 3:
		writeDisplayByCode(RS_INSTRUCTION, SET_DDRAM_ADDR
				| (LINE4_FIRST_CHARACTER_ADDRESS + x));
		portDelay(1);
		break;
	}
}

void displayWrite( char const * str )
{
	while(*str)
	{
		writeDisplayByCode(RS_DATA, *str++);
	}
}

// 4 bit mode
void writeDisplayData(uint8_t data)
{
	writeExpanderPin(EN, 0);
	writeExpanderPin(D7, data & 0b10000000);
	writeExpanderPin(D6, data & 0b01000000);
	writeExpanderPin(D5, data & 0b00100000);
	writeExpanderPin(D4, data & 0b00010000);
	writeExpanderPin(EN, 1);
	portDelay(5);
	writeExpanderPin(EN, 0);
	portDelay(5);
	writeExpanderPin(D7, data & 0b00001000);
	writeExpanderPin(D6, data & 0b00000100);
	writeExpanderPin(D5, data & 0b00000010);
	writeExpanderPin(D4, data & 0b00000001);
	writeExpanderPin(EN, 1);
	portDelay(5);
	writeExpanderPin(EN, 0);
	portDelay(5);
}

void writeDisplayByCode(bool code, uint8_t data)
{
	writeExpanderPin(RS, code);
	writeExpanderPin(RW, RW_WRITE);
	writeDisplayData(data);
}

void writeExpanderPin(i2cExpanderPin_t pin, bool value)
{
	if(!value)
		dataExpander &= ~(0x01 << pin);
	else
		dataExpander |= 0x01 << pin;
	portI2Cwrite(I2C_ADDRESS, &dataExpander, EXPANDER_DATA_SIZE);
}
