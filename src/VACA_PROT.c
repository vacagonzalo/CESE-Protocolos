#include "VACA_PROT.h"
#include "sapi.h"
#include "VACA_DISPLAY.h"

int main( void )
{
	boardInit();
	displayInit();

	displayCursorPos( 0, 0 );
	displayWrite("Reinicio y espera");
	displayCursorPos( 0, 1 );
	displayWrite("de 1 segundo");
	displayCursorPos( 0, 2 );
	displayWrite("para demostracion");
	displayCursorPos( 0, 3 );
	displayWrite("FIUBA LSE - CESE 15");
	delay(1000);

	displayClear();
	delay(500);

	displayCursorPos( 0, 0 );
	displayWrite("Protocolos de comuni");
	displayCursorPos( 4, 1 );
	displayWrite("Gonzalo Vaca");
	displayCursorPos( 2, 2 );
	displayWrite("HITACHI HD44780U");
	displayCursorPos( 0, 3 );
	displayWrite("i2c expansio PCF8574");
	while( true ) {
		gpioToggle(LED1);
		delay(500);
	}
	return 0;
}
