#include "pcf8563.h"

/*
TO READ THE TIME

Send a START condition and the slave address for write (A2h). 2. Set the address pointer to 2 (VL_seconds) by sending 02h.
3. Send a RESTART condition or STOP followed by START.
4. Send the slave address for read (A3h).
5. Read VL_seconds. 6. Read Minutes.
7. Read Hours.
8. Read Days.
9. Read Weekdays.
10. Read Century_months. 11. Read Years.
12. Send a STOP condition.
*/

void pcf8563Init(void)
{
    // clear control status 1
    uint8_t wbuf[1];
    uint8_t wbuf2[1];
    wbuf[0] = Control_status_1;
    wbuf2[0] = 0x00;
    i2c_io(PCF8563_address, wbuf, 1, wbuf2, 1, NULL, 0);
    
    // clear control status 2
    wbuf[0] = Control_status_2;
    wbuf2[0] = 0x00;
    i2c_io(PCF8563_address, wbuf, 1, wbuf2, 1, NULL, 0);
    
    
    
    
    
    
    
    // // clear TESTC bit (Control_status_1 &= ~(1<<3)))
    // unsigned char wbuf[1];
    // wbuf[0] = Control_status_1;
    // unsigned char wbuf2[1];
    // wbuf2[0] = 0x00;
    // unsigned char rbuf[1];
    // i2c_io(PCF8563_address, wbuf, 1, NULL, 0, rbuf, 1);
    // rbuf[0] &= ~(1 << 3);
    // // write to clear TESTC bit
    // i2c_io(PCF8563_address, wbuf, 1, rbuf, 1, NULL, 0);

    // // clear CLKOUT enable bit ((CLKOUT_control,~(1<<7)))

    // wbuf[0] = CLKOUT_control;
    // i2c_io(PCF8563_address, wbuf, 1, NULL, 0, rbuf, 1);
    // rbuf[0] &= ~(1 << 7);
    // // write to clear TESTC bit
    // i2c_io(PCF8563_address, wbuf, 1, rbuf, 1, NULL, 0);
}

// Start the clock
// Parameters: None
// Returns:    None
void startClock(void)
{
    // write_AND(Control_status_1,~(1<<5));
    unsigned char wbuf[1];
    wbuf[0] = Control_status_1;
    unsigned char rbuf[1];
    i2c_io(PCF8563_address, wbuf, 1, NULL, 0, rbuf, 1);
    rbuf[0] &= ~(1 << 5);
    // write to clear TESTC bit
    i2c_io(PCF8563_address, wbuf, 1, rbuf, 1, NULL, 0);
}

// Start the clock
// Parameters: None
// Returns:    None
void stopClock(void)
{

    // write_OR(Control_status_1,1<<5);
    unsigned char wbuf[1];
    wbuf[0] = Control_status_1;
    unsigned char rbuf[1];
    i2c_io(PCF8563_address, wbuf, 1, NULL, 0, rbuf, 1);
    rbuf[0] |= 1 << 5;
    // write to clear TESTC bit
    i2c_io(PCF8563_address, wbuf, 1, rbuf, 1, NULL, 0);
}

// Set the year
// Parameters:
//  * uint8_t year -> selected year (you can set values 0-99)
// Returns: None
void setYear(uint8_t year)
{
    uint8_t data[1];
    data[0] = ((get_second_number(year)) << 4) | (get_first_number(year));
    uint8_t wbuf[1];
    wbuf[0] = Years;
    i2c_io(PCF8563_address, wbuf, 1, data, 1 , NULL, 0);
    
}

// Set the month
// Parameters:
//  * uint8_t month -> selected month (you can set values 1-12)
// Returns: None
void setMonth(uint8_t month)
{
    uint8_t data[1];
    data[0] = ((get_second_number(month)) << 4) | (get_first_number(month));
    uint8_t wbuf[1];
    wbuf[0] = Century_months;
    i2c_io(PCF8563_address, wbuf, 1, data, 1 , NULL, 0);
}

// Set the day
// Parameters:
//  * uint8_t day -> selected day (you can set values 1-31)
// Returns: None
void setDay(uint8_t day)
{
    uint8_t data[1];
    data[0] = ((get_second_number(day)) << 4) | (get_first_number(day));
    uint8_t wbuf[1];
    wbuf[0] = Days;
    i2c_io(PCF8563_address, wbuf, 1, data, 1 , NULL, 0);
}

// Set the hour
// Parameters:
//  *uint8_t hour -> selected day (you can set values 0-24)
// Returns: None
void setHour(uint8_t hour)
{
    uint8_t data[1];
    data[0] = ((get_second_number(hour)) << 4) | (get_first_number(hour));
    uint8_t wbuf[1];
    wbuf[0] = Hours;
    i2c_io(PCF8563_address, wbuf, 1, data, 1 , NULL, 0);
}

// Set the minut
// Parameters:
//  *uint8_t minut -> selected day (you can set values 0-59)
// Returns:    None
void setMinute(uint8_t minut)
{
    uint8_t data[1];
    data[0] = ((get_second_number(minut)) << 4) | (get_first_number(minut));
    uint8_t wbuf[1];
    wbuf[0] = Minutes;
    i2c_io(PCF8563_address, wbuf, 1, data, 1 , NULL, 0);
}

// Set the second
// Parameters:
//  * uint8_t second -> selected day (you can set values 0-59)
// Returns: None
void setSecond(uint8_t second)
{
    uint8_t data[1];
    data[0] = ((get_second_number(second)) << 4) | (get_first_number(second));
    uint8_t wbuf[1];
    wbuf[0] = VL_seconds;
    i2c_io(PCF8563_address, wbuf, 1, data, 1 , NULL, 0);
   
}

// Get current time
// Parameters: None
// Returns: Time (current code encoded into the Time structure)
void getTime(unsigned char *rbuf_time, int* current_time )
{

    // read data registers contents
    //    uint8_t YEAR    = read(Years);

    uint8_t wbuf_year[1];
    wbuf_year[0] = Years;
    uint8_t rbuf_year[1];
    i2c_io(PCF8563_address, wbuf_year, 1, NULL, 0, rbuf_year, 1);

    //    uint8_t MONTH   = read(Century_months);
    uint8_t wbuf_month[1];
    wbuf_month[0] = Century_months;
    uint8_t rbuf_month[1];
    i2c_io(PCF8563_address, wbuf_month, 1, NULL, 0, rbuf_month, 1);

    //    uint8_t DAY     = read(Days);

    uint8_t wbuf_day[1];
    wbuf_day[0] = Days;
    uint8_t rbuf_day[1];
    i2c_io(PCF8563_address, wbuf_day, 1, NULL, 0, rbuf_day, 1);

    //    uint8_t WEEKDAY = read(Weekdays);

    uint8_t wbuf_weekday[1];
    wbuf_weekday[0] = Weekdays;
    uint8_t rbuf_weekday[1];
    i2c_io(PCF8563_address, wbuf_weekday, 1, NULL, 0, rbuf_weekday, 1);
    //    uint8_t HOUR    = read(Hours);

    uint8_t wbuf_hour[1];
    wbuf_hour[0] = Hours;
    uint8_t rbuf_hour[1];
    i2c_io(PCF8563_address, wbuf_hour, 1, NULL, 0, rbuf_hour, 1);

    //    uint8_t MINUTE  = read(Minutes);

    uint8_t wbuf_min[1];
    wbuf_min[0] = Minutes;
    uint8_t rbuf_min[1];
    i2c_io(PCF8563_address, wbuf_min, 1, NULL, 0, rbuf_min, 1);

    //    uint8_t SECONDS = read(VL_seconds);
    uint8_t wbuf_sec[1];
    wbuf_sec[0] = VL_seconds;
    uint8_t rbuf_sec[1];
    i2c_io(PCF8563_address, wbuf_sec, 1, NULL, 0, rbuf_sec, 1);

    unsigned char rbuf[7];

    // convert readed data to numbers using bcd_to_number function).
    rbuf[0] = bcd_to_number((rbuf_year[0] & 0b11110000) >> 4, rbuf_year[0] & 0b00001111);
    rbuf[1] = bcd_to_number((rbuf_month[0] & 0b00010000) >> 4, rbuf_month[0] & 0b00001111);
    rbuf[2] = bcd_to_number((rbuf_day[0] & 0b00110000) >> 4, rbuf_day[0] & 0b00001111);
    rbuf[3] = bcd_to_number(0, rbuf_weekday[0] & 0b00000111);
    rbuf[4] = bcd_to_number((rbuf_hour[0] & 0b00110000) >> 4, rbuf_hour[0] & 0b00001111);
    rbuf[5] = bcd_to_number((rbuf_min[0] & 0b01110000) >> 4, rbuf_min[0] & 0b00001111);
    rbuf[6] = bcd_to_number((rbuf_sec[0] & 0b01110000) >> 4, rbuf_sec[0] & 0b00001111);

    // snprintf(rbuf_time, 20, "Time:%d/%d/%d,%d:%d", rbuf[1], rbuf[2], rbuf[0], rbuf[4], rbuf[5]);
    snprintf(rbuf_time, 6, "%02d:%02d", rbuf[4], rbuf[5]);

    current_time[0] = (int)rbuf[5];
    current_time[1] = (int)rbuf[6];
    return ;
}

void getDate(int* current_date) {

    // read data registers contents
    //    uint8_t YEAR    = read(Years);

    uint8_t wbuf_year[1];
    wbuf_year[0] = Years;
    uint8_t rbuf_year[1];
    i2c_io(PCF8563_address, wbuf_year, 1, NULL, 0, rbuf_year, 1);

    //    uint8_t MONTH   = read(Century_months);
    uint8_t wbuf_month[1];
    wbuf_month[0] = Century_months;
    uint8_t rbuf_month[1];
    i2c_io(PCF8563_address, wbuf_month, 1, NULL, 0, rbuf_month, 1);

    //    uint8_t DAY     = read(Days);

    uint8_t wbuf_day[1];
    wbuf_day[0] = Days;
    uint8_t rbuf_day[1];
    i2c_io(PCF8563_address, wbuf_day, 1, NULL, 0, rbuf_day, 1);

    //    uint8_t WEEKDAY = read(Weekdays);

    uint8_t wbuf_weekday[1];
    wbuf_weekday[0] = Weekdays;
    uint8_t rbuf_weekday[1];
    i2c_io(PCF8563_address, wbuf_weekday, 1, NULL, 0, rbuf_weekday, 1);
    //    uint8_t HOUR    = read(Hours);

    uint8_t wbuf_hour[1];
    wbuf_hour[0] = Hours;
    uint8_t rbuf_hour[1];
    i2c_io(PCF8563_address, wbuf_hour, 1, NULL, 0, rbuf_hour, 1);

    //    uint8_t MINUTE  = read(Minutes);

    uint8_t wbuf_min[1];
    wbuf_min[0] = Minutes;
    uint8_t rbuf_min[1];
    i2c_io(PCF8563_address, wbuf_min, 1, NULL, 0, rbuf_min, 1);

    //    uint8_t SECONDS = read(VL_seconds);
    uint8_t wbuf_sec[1];
    wbuf_sec[0] = VL_seconds;
    uint8_t rbuf_sec[1];
    i2c_io(PCF8563_address, wbuf_sec, 1, NULL, 0, rbuf_sec, 1);

    unsigned char rbuf[7];

    // convert readed data to numbers using bcd_to_number function).
    rbuf[0] = bcd_to_number((rbuf_year[0] & 0b11110000) >> 4, rbuf_year[0] & 0b00001111);
    rbuf[1] = bcd_to_number((rbuf_month[0] & 0b00010000) >> 4, rbuf_month[0] & 0b00001111);
    rbuf[2] = bcd_to_number((rbuf_day[0] & 0b00110000) >> 4, rbuf_day[0] & 0b00001111);
    rbuf[3] = bcd_to_number(0, rbuf_weekday[0] & 0b00000111);
    rbuf[4] = bcd_to_number((rbuf_hour[0] & 0b00110000) >> 4, rbuf_hour[0] & 0b00001111);
    rbuf[5] = bcd_to_number((rbuf_min[0] & 0b01110000) >> 4, rbuf_min[0] & 0b00001111);
    rbuf[6] = bcd_to_number((rbuf_sec[0] & 0b01110000) >> 4, rbuf_sec[0] & 0b00001111);
    
    current_date[0] = (int)rbuf[2];
    current_date[1] = (int)rbuf[1];

    current_date[2] = (int)rbuf[4];
    current_date[3] = (int)rbuf[5];
    current_date[4] = (int)rbuf[6];
}

// Convert BCD format to number
// Parameters:
//  * uint8_t first -> first digit
//  * uint8_t second -> second digit
// Returns: the result of the conversion (unsigned char)
unsigned char bcd_to_number(uint8_t first, uint8_t second)
{
    unsigned char output;
    output = first * 10;
    output = output + second;
    return output;
}

// Get first digit of the number
// Parameters:
//  * unsigned short ->
// Returns: digit (uint8_t)
uint8_t get_first_number(unsigned short number)
{
    uint8_t output = number % 10;
    return output;
}

// Get second digit of the number
// Parameters:
//  * unsigned short ->
// Returns: digit (uint8_t)
uint8_t get_second_number(unsigned short number)
{
    uint8_t output = number / 10;
    return output;
}
