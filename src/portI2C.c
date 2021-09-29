#include "portI2C.h"
#include "sapi.h"

#define DEFAULT_I2C I2C0
#define SPEED_100KHZ 100000
#define NO_STOP 0
#define BUFFER_SIZE 1

uint8_t as_signed(uint8_t s);

void portI2Cinit()
{
	i2cInit( DEFAULT_I2C, SPEED_100KHZ );
}

void portI2Cwrite(uint8_t addr, uint8_t* data)
{
	i2cWrite( DEFAULT_I2C, as_signed(addr), data, BUFFER_SIZE, NO_STOP);
}

void portI2CwriteChar(uint8_t addr, char* data)
{
	i2cWrite( DEFAULT_I2C, as_signed(addr), data, BUFFER_SIZE, NO_STOP);
}

void portDelay(uint32_t ms)
{
	delay(ms);
}

uint8_t as_signed(uint8_t s)
{
	uint8_t lsb = (s & 0b00000001) << 7;
	s = s >> 1;
	s |= lsb;
	return s;
}
