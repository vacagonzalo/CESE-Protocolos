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

// WRITE: 64 66 68 70 72 74 76 78
// se debió corregir el port, número signado en hoja de datos
// i2c maneja los addr como uint8_t
#define WRITE_ADDRESS 78
#define EXPANDER_DATA_SIZE 1

typedef enum {
	RS, RW, EN, AA, D4, D5, D6, D7
} i2cExpanderPin_t;

typedef struct {
	uint8_t addr;
	char data;
	bool pin_RS;
	bool pin_RW;
	bool pin_EN;
	bool pin_A;
	bool pin_D4;
	bool pin_D5;
	bool pin_D6;
	bool pin_D7;

}expander_t;

expander_t expa;

static uint8_t dataExpander = 0x00;

void writeExpanderPin(i2cExpanderPin_t pin, bool value);
void writeDisplayByCode(bool code, uint8_t data);
void writeDisplayData(uint8_t data);

// El HD44780U comienza en modo 8 bits.
// Los 1ros 4 mensajes de inicialización los tengo que mandar
// ignorando los 4 bits menos significativos
void writeDisplayByCode8bitMode(bool code, uint8_t data);
void writeDisplayData8bitMode(uint8_t data);

void displayInit()
{
	portI2Cinit();

	expa.addr   = WRITE_ADDRESS;
	expa.data   = 0b00000000;
	expa.pin_RS = 0;
	expa.pin_RW = 0;
	expa.pin_EN = 0;
	expa.pin_A  = 0;
	expa.pin_D4 = 0;
	expa.pin_D5 = 0;
	expa.pin_D6 = 0;
	expa.pin_D7 = 0;

	///////////////////////////////////////////////////////////////////////////
	// Debug de pines de control
	//writeExpanderPin(RS, 1);
	//writeExpanderPin(RW, 1);
	//writeExpanderPin(EN, 1);
	//writeExpanderPin(AA, 1);
	//writeExpanderPin(RS, 0);
	//writeExpanderPin(RW, 0);
	//writeExpanderPin(EN, 0);
	//writeExpanderPin(AA, 0);
	//portDelay(5);
	//writeExpanderPin(RS, 1);
	//writeExpanderPin(RW, 1);
	//writeExpanderPin(EN, 1);
	//writeExpanderPin(AA, 1);
	//writeExpanderPin(RS, 0);
	//writeExpanderPin(RW, 0);
	//writeExpanderPin(EN, 0);
	//writeExpanderPin(AA, 0);
	// AA funciona, pero no lo veo en el analizador lógico.
	///////////////////////////////////////////////////////////////////////////
	bool initial = 0;
	writeExpanderPin(AA, 1);
	portDelay(50); // doy tiempo al startup del HD44780U

	// Instrucciones para inicialización según Hitachi (pag 23)
	//    Designate HD44780U functions, such as display format, data length, etc.
	//    Set internal RAM addresses
	//    Perform data transfer with internal RAM
	//    Perform miscellaneous functions
	writeDisplayByCode8bitMode(RS_INSTRUCTION, FUNCTION_SET
			| FUNCTION_SET_8BITS);
	portDelay(5);

	writeDisplayByCode8bitMode(RS_INSTRUCTION, FUNCTION_SET
			| FUNCTION_SET_8BITS);
	portDelay(1);

	writeDisplayByCode8bitMode(RS_INSTRUCTION, FUNCTION_SET
			| FUNCTION_SET_8BITS);
	portDelay(1);

	writeDisplayByCode8bitMode(RS_INSTRUCTION, FUNCTION_SET
			| FUNCTION_SET_4BITS);
	portDelay(1);
	initial = 1;
	///////////////////////////////////////////////////////////////////////////

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
		break;
	case 1:
		writeDisplayByCode(RS_INSTRUCTION, SET_DDRAM_ADDR
				| (LINE2_FIRST_CHARACTER_ADDRESS + x));
		break;
	case 2:
		writeDisplayByCode(RS_INSTRUCTION, SET_DDRAM_ADDR
				| (LINE3_FIRST_CHARACTER_ADDRESS + x));
		break;
	case 3:
		writeDisplayByCode(RS_INSTRUCTION, SET_DDRAM_ADDR
				| (LINE4_FIRST_CHARACTER_ADDRESS + x));
		break;
	}
	portDelay(1);
}

// Funciona, envía el hexa correspondiente al caracter
// recorre todo el str hasta encontrar \0
void displayWrite( char const * str )
{
	while(*str)
	{
		writeDisplayByCode(RS_DATA, *str++);
	}
}

// 4 bit mode (4 bits son de control)
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
	// code:
	// RS_INSTRUCTION 0
	// RS_DATA 1
	writeExpanderPin(RS, code); // elijo enviar comando o caracter

	// RW_WRITE 0
	// RW_READ 1
	writeExpanderPin(RW, RW_WRITE); // Nunca voy a leer
	writeDisplayData(data);
}

// Funciona, el único que no cambia es pin_A
void writeExpanderPin(i2cExpanderPin_t pin, bool value)
{
	switch(pin)
	{
	case RS: expa.pin_RS = value; break;
	case RW: expa.pin_RW = value; break;
	case EN: expa.pin_EN = value; break;
	case AA: expa.pin_A  = value; break;
	case D4: expa.pin_D4 = value; break;
	case D5: expa.pin_D5 = value; break;
	case D6: expa.pin_D6 = value; break;
	case D7: expa.pin_D7 = value; break;
	default: break;
	}
	expa.data = 0b00000000;
	if (expa.pin_RS) expa.data |= 0b00000001;
	if (expa.pin_RW) expa.data |= 0b00000010;
	if (expa.pin_EN) expa.data |= 0b00000100;
	if (expa.pin_A ) expa.data |= 0b00001000;
	if (expa.pin_D4) expa.data |= 0b00010000;
	if (expa.pin_D5) expa.data |= 0b00100000;
	if (expa.pin_D6) expa.data |= 0b01000000;
	if (expa.pin_D7) expa.data |= 0b10000000;
	portI2Cwrite(expa.addr,&expa.data);
}

void writeDisplayByCode8bitMode(bool code, uint8_t data)
{
	// code:
		// RS_INSTRUCTION 0
		// RS_DATA 1
		writeExpanderPin(RS, code); // elijo enviar comando o caracter

		// RW_WRITE 0
		// RW_READ 1
		writeExpanderPin(RW, RW_WRITE); // Nunca voy a leer
		writeDisplayData8bitMode(data);

}

void writeDisplayData8bitMode(uint8_t data)
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
}
