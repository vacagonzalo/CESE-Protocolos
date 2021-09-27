#include "VACA_PROT.h"
#include "sapi.h"
#include "VACA_DISPLAY.h"
int main( void )
{
	boardInit();
	displayInit();
	displayCursorPos(0, 0);
	displayWrite("GONZALO VACA");
	while( true ) {
		gpioToggle(LED1);
		delay(500);
	}
	return 0;
}
