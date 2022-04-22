#include "ds1631.h"
#include <util/delay.h>

// Initialize Sensor by setting config to default
//(Power-up state: 100011XX (XX = user defined))
//  XX: POL and 1SHOT bits => we set to 10
void ds1631_init()
{
    //unsigned char buf[1] = {DS1631_CONFIG_1SHOT};
    unsigned char buf[1] = {DS1631_CONFIG_DEFAULT};
    ds1631_set_config(buf);
}

/*
    Updates config register to specified byte
*/
void ds1631_set_config(const unsigned char *config)
{
    uint8_t wbuf[1] = {DS1631_ACCESS_CONFIG}; // config register address
    unsigned char config2[1];
    config2[0] = ds1631_get_config();
    config2[0] &= config[0];
    i2c_io(DS1631_ID, wbuf, 1, config2, 1, NULL, 0); // Send config to sensor configuration register
}

unsigned char ds1631_get_config()
{
    unsigned char wbuf[1] = {DS1631_ACCESS_CONFIG}; // write the DS1631 configuration byte
    unsigned char rbuf[1];

    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, rbuf, 1); // Read sensor config into rbuf

    return rbuf[0];
}
/*
    Sends specified byte as command to DS1631 w/ no read
*/
void ds1631_write_command(const unsigned char command, unsigned char *data)
{
    if (data == NULL)
    {
        unsigned char wbuf[1] = {command};
        i2c_io(DS1631_ID, wbuf, 1, NULL, 0, NULL, 0);
    }
    else
    {
        unsigned char wbuf[1] = {command};
        i2c_io(DS1631_ID, wbuf, 1, data, 2, NULL, 0);
    }
}

/*
    Sends specified byte as command to DS1631 w/ 2 bytes read back
*/

void ds1631_read_command(const unsigned char command, unsigned char *rbuf)
{
    unsigned char wbuf[1] = {command};
    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, rbuf, 2);
}

/*
    Start a temperature conversion
    Note: Only necessary while in oneShotMode
*/
void ds1631_start_convert()
{
    ds1631_write_command(DS1631_START_CONV, NULL);
}
void ds1631StopConvert()
{
    ds1631_write_command(DS1631_STOP_CONV, NULL);
}

void ds1631SetTH(uint8_t temp_high, uint8_t temp_low)
{
    // unsigned char temp[2];
    // temp[0] = 0;
    // temp[1] = 0;
    // ds1631_write_command(DS1631_SET_TH, temp);

    // set the 16 bit value for the low threshold also reset the given threshold

    unsigned char command[1];
    uint8_t msb = temp_high;
    uint8_t lsb = 0;


    uint8_t temp[2] = {msb, lsb};
    command[0] = DS1631_SET_TH;
    // set high threshold
    // ds1631_write_command(DS1631_SET_TH, (unsigned char) temp)
    i2c_io(DS1631_ID, command, 1, temp, 2, NULL, 0);

    // Set low threshold
    
    command[0] = DS1631_SET_TL;
    // ds1631_write_command(DS1631_SET_TL, (unsigned char) temp)
    i2c_io(DS1631_ID, command, 1, temp, 2, NULL, 0);
}

void ds1631ReadTH(unsigned char *th_buf, unsigned char* th_buf_low)
{
    unsigned char wbuf[1];
    wbuf[0] = DS1631_SET_TH;
    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, th_buf, 2);
    
  
    wbuf[0] = DS1631_SET_TL;
    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, th_buf_low, 2);
}

unsigned char ds1631_conversion_status()
{
    unsigned char rbuf;
    unsigned char wbuf[1] = {DS1631_ACCESS_CONFIG};
    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, &rbuf, 1); // Read sensor config into rbuf

    if (rbuf & 0x80)
    { // Check MSB of config register
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
    Read temp data (2 bytes) into buf
*/
void ds1631_read_temp(unsigned char *rbuf)
{
    if(0) {
        while(!ds1631_conversion_status()) {

        }
    }
    ds1631_read_command(DS1631_READ_TEMP, rbuf);
}

void ds1631_formatted_temp(char* temp_string) {
    unsigned char temp[2];
    unsigned char temp2[2];
    ds1631_read_temp(temp);             // Read temp from DS1631
    _delay_ms(50);
    ds1631_read_temp(temp2);
    if(temp[0] == temp2[0]) {
        snprintf(temp_string, 6, "%2u%cC", temp[0], 223); // Format string i.e. "Temp: 25 C"
    }
}
