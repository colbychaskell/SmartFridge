#ifndef DS1631_H
#define DS1631_H

#include <stdio.h>
#include <string.h>
#include "i2c.h"

#define DS1631_ID 0x90

// Commands
#define DS1631_START_CONV 0x51
#define DS1631_STOP_CONV 0x22
#define DS1631_READ_TEMP 0xAA
#define DS1631_ACCESS_CONFIG 0xAC
#define DS1631_SWPOR 0x54
#define DS1631_SET_TH 0xA1

// Configuration Register Values
// #define DS1631_CONFIG_DEFAULT 0x02
#define DS1631_CONFIG_DEFAULT 0xFE
#define DS1631_CONFIG_1SHOT 0x01
#define DS1631_CONFIG_POL 0x02
#define DS1631_CONFIG_1SHOT_POL 0x03
#define DS1631_CONFIG_R0 0x04
#define DS1631_CONFIG_R1 0x08
#define DS1631_CONFIG_NVB 0x10
#define DS1631_CONFIG_TLF 0x20
#define DS1631_CONFIG_THF 0x40
#define DS1631_CONFIG_DONE 0x80

#define OSTR_SIZE   80

// Initialization
void ds1631_init();                                   // Initialize sensor without default configuration
void ds1631_set_config(const unsigned char* config); // Specify a byte to put into sensor config register
unsigned char ds1631_get_config();

// Command Helper Functions
void ds1631_read_command(const unsigned char command, unsigned char* rbuf);
void ds1631_write_command(const unsigned char command, unsigned char* data);

// Reading Temperature
void ds1631_start_convert(); // Starts a temperature conversion if in one shot mode
void ds1631_stop_convert();
unsigned char ds1631_convert_status(); // Check if one-shot temperature conversion has finished
void ds1631_read_temp(unsigned char* rbuf); // Returns the temperature
void ds1631SetTH(uint8_t temp);
void ds1631ReadTH(uint8_t* th_buf);
#endif
