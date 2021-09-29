#include "VACA_PROT.h"
#include "sapi.h"
#include "VACA_DISPLAY.h"

int main( void )
{
	boardInit();
	displayInit();
	displayCursorPos( 0, 0 );
	displayWrite("Aloha!");
	delay(1000);

	displayCursorPos( 0, 0 );
	displayWrite("Protocolos de co");
	displayCursorPos( 0, 1 );
	displayWrite("Gonzalo Vaca");
	displayCursorPos( 0, 2 );
	displayWrite("HITACHI HD44780U");
	displayCursorPos( 0, 3 );
	displayWrite("i2c expa PCF8574");
	while( true ) {
		gpioToggle(LED1);
		delay(500);
	}
	return 0;
}
