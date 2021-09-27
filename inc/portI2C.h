#ifndef PROGRAMS_VACA_PROT_INC_PORTI2C_H_
#define PROGRAMS_VACA_PROT_INC_PORTI2C_H_

#include<stdint.h>

void portI2Cinit();
void portI2Cwrite(uint8_t addr, uint8_t* data, uint16_t size);
void portDelay(uint32_t ms);

#endif /* PROGRAMS_VACA_PROT_INC_PORTI2C_H_ */
