

#ifndef PCF8563_H
#define PCF8563_H


#include <stdio.h>
#include <string.h>
#include "i2c.h"

#define DEVICE_ADDR 0xA2

// REG ADDR
#define PCF8563_address   0xA2
#define Control_status_1  0x00
#define Control_status_2  0x01
#define VL_seconds        0x02
#define Minutes           0x03
#define Hours             0x04
#define Days              0x05
#define Weekdays          0x06
#define Century_months    0x07
#define Years             0x08
#define Minute_alarm      0x09
#define Hour_alarm        0x0A
#define Day_alarm         0x0B
#define Weekday_alarm     0x0C
#define CLKOUT_control    0x0D
#define Timer_control     0x0E
#define Timer             0x0F

#define OSTR_SIZE   80

// Functions

void pcf8563Init(void);
void startClock(void);
void stopClock(void);
void setYear(uint8_t year);
void setMonth(uint8_t month);
void setDay(uint8_t day);
void setHour(uint8_t hour);
void setMinut(uint8_t minut);
void setSecond(uint8_t second);
void getTime(unsigned char *rbuf, int* minAndSec);
unsigned char bcd_to_number(uint8_t first, uint8_t second);
uint8_t get_first_number(unsigned short number);
uint8_t get_second_number(unsigned short number);


#endif