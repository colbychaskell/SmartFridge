#include "ds1631.h"

// Initialize Sensor by setting config to default
//(Power-up state: 100011XX (XX = user defined))
//  XX: POL and 1SHOT bits => we set to 10
void ds1631_init()
{
    unsigned char buf[1] = {DS1631_CONFIG_1SHOT};
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
    unsigned char rbuf2[2];

    char ostr[OSTR_SIZE]; // Buffer for creating strings

    unsigned char wbuf[1] = {command};
    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, rbuf2, 2);
    snprintf(ostr, OSTR_SIZE, "Temp: %d%d", rbuf2[0], rbuf2[1]);

    rbuf[0] = ostr[0];
    rbuf[1] = ostr[1];
    rbuf[2] = ostr[2];
    rbuf[3] = ostr[3];
    rbuf[4] = ostr[4];
    rbuf[5] = ostr[5];
    rbuf[6] = ostr[6];
    rbuf[7] = ostr[7];
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

void ds1631SetTH(uint8_t temp)
{
    // unsigned char temp[2];
    // temp[0] = 0;
    // temp[1] = 0;
    // ds1631_write_command(DS1631_SET_TH, temp);
    
    // set the 16 bit value for the low threshold also reset the given threshold
    
    unsigned char wbuf[1];
    uint8_t data[2];
    wbuf[0] = DS1631_SET_TH;
    data[0] = temp;
    data[1] = 0;
    // set high threshold
    i2c_io(DS1631_ID, wbuf, 1, data, 2, NULL , 0);
  
}

void ds1631ReadTH(unsigned char* th_buf)
{
    unsigned char wbuf[1];
    wbuf[0] = DS1631_SET_TH;
    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, th_buf , 2);
    
 
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
    unsigned char config = ds1631_get_config();
    // //int temperature;
    // //unsigned char temperature;
    if (config & 0x01)
    { // If we are in one-shot mode then wait for conversion, otherwise read now
        while (!ds1631_conversion_status())
        {
            // Wait for conversion to complete
        }
    }
    ds1631_read_command(DS1631_READ_TEMP, rbuf);
}
