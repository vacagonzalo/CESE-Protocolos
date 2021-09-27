#include "portI2C.h"
#include "sapi.h"

#define DEFAULT_I2C I2C0
#define SPEED_100KHZ 100000
#define STOP 0

void portI2Cinit()
{
	i2cInit( DEFAULT_I2C, SPEED_100KHZ );
}

void portI2Cwrite(uint8_t addr, uint8_t* data, uint16_t size)
{
	i2cWrite( DEFAULT_I2C, addr, data, size, STOP);
}

void portDelay(uint32_t ms)
{
	delay(ms);
}
