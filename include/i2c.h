#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define FOSC  9830400 // Clock frequency = Oscillator freq.
#define BDIV  (FOSC / 100000 - 16) / 2 + 1

extern void i2c_init(uint8_t);
extern uint8_t i2c_io(uint8_t, uint8_t *, uint16_t,
		      uint8_t *, uint16_t, uint8_t *, uint16_t);

#endif
