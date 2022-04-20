#ifndef TSL2591_H
#define TSL2591_H

#include <stdio.h>
#include <string.h>
#include "i2c.h"





#define TSL2591_VISIBLE (2)      ///< (channel 0) - (channel 1)
#define TSL2591_INFRARED (1)     ///< channel 1
#define TSL2591_FULLSPECTRUM (0) ///< channel 0

#define TSL2591_ADDR (0x52) ///< Default I2C address

#define TSL2591_COMMAND_BIT                                                    \
  (0xA0) ///< 1010 0000: bits 7 and 5 for 'command normal'

///! Special Function Command for "Clear ALS and no persist ALS interrupt"
#define TSL2591_CLEAR_INT (0xE7)
///! Special Function Command for "Interrupt set - forces an interrupt"
#define TSL2591_TEST_INT (0xE4)

#define TSL2591_WORD_BIT (0x20)  ///< 1 = read/write word (rather than byte)
#define TSL2591_BLOCK_BIT (0x10) ///< 1 = using block read/write

#define TSL2591_ENABLE_POWEROFF (0x00) ///< Flag for ENABLE register to disable
#define TSL2591_ENABLE_POWERON (0x01)  ///< Flag for ENABLE register to enable
#define TSL2591_ENABLE_AEN                                                     \
  (0x02) ///< ALS Enable. This field activates ALS function. Writing a one
         ///< activates the ALS. Writing a zero disables the ALS.
#define TSL2591_ENABLE_AIEN                                                    \
  (0x10) ///< ALS Interrupt Enable. When asserted permits ALS interrupts to be
         ///< generated, subject to the persist filter.
#define TSL2591_ENABLE_NPIEN                                                   \
  (0x80) ///< No Persist Interrupt Enable. When asserted NP Threshold conditions
         ///< will generate an interrupt, bypassing the persist filter

#define TSL2591_LUX_DF (408.0F)   ///< Lux cooefficient
#define TSL2591_LUX_COEFB (1.64F) ///< CH0 coefficient
#define TSL2591_LUX_COEFC (0.59F) ///< CH1 coefficient A
#define TSL2591_LUX_COEFD (0.86F) ///< CH2 coefficient B


#define TSL2591_REGISTER_CHAN0_LOW  0x14      // Channel 0 data, low byte
#define TSL2591_REGISTER_CHAN0_HIGH  0x15    // Channel 0 data, high byte
#define TSL2591_REGISTER_CHAN1_LOW  0x16     // Channel 1 data, low byte
#define TSL2591_REGISTER_CHAN1_HIGH  0x17     // Channel 1 data, high byte



#define OSTR_SIZE   80


// float calculateLux(uint16_t ch0, uint16_t ch1);

float readLight(unsigned char* rbuf);
void tsl2591_init(void);

float calculateLux(uint16_t ch0, uint16_t ch1);




#endif
