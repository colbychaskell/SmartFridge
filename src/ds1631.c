#include "ds1631.h"

//Initialize Sensor by setting config to default
void ds1631_init() {
    unsigned char buf[1] = {DS1631_CONFIG_DEFAULT};
    ds1631_set_config(buf);
}
/*
    Updates config register to specified byte
*/
void ds1631_set_config(const unsigned char* config) {
    uint8_t wbuf[1] = {DS1631_ACCESS_CONFIG};
    i2c_io(DS1631_ID, wbuf, 1, config, 1, NULL, 0); // Send config to sensor configuration register
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
void ds1631_write_command(const unsigned char command) {
    unsigned char wbuf[1] = {command};
    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, NULL, 0);
}

/*
    Sends specified byte as command to DS1631 w/ 2 bytes read back
*/
unsigned char ds1631_read_command(const unsigned char command)
{
    unsigned char rbuf;
    unsigned char wbuf[1] = {command};
    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, &rbuf, 2);
    return rbuf;
}

/*
    Start a temperature conversion
    Note: Only necessary while in oneShotMode
*/
void ds1631_start_convert() {
    ds1631_write_command(DS1631_START_CONV);
}
void ds1631StopConvert()
{
    ds1631_write_command(DS1631_STOP_CONV);
}

unsigned char ds1631_conversion_status() {
    unsigned char rbuf;
    unsigned char wbuf[1] = {DS1631_ACCESS_CONFIG};
    i2c_io(DS1631_ID, wbuf, 1, NULL, 0, &rbuf, 1); // Read sensor config into rbuf

    if(rbuf & 0x80) { //Check MSB of config register
        return 1;
    }
    else{
        return 0;
    }
}

/*
    Read temp data (2 bytes) into buf
*/
int ds1631_read_temp() {
    unsigned char config = ds1631_get_config();
    //int temperature;
    unsigned char temperature;
    // if (config & 0x01) { // If we are in one-shot mode then wait for conversion, otherwise read now
    //     while (!ds1631_conversion_status()) {
    //         // Wait for conversion to complete
    //     }                                                                         
    // }
    temperature = ds1631_read_command(DS1631_READ_TEMP);
    // _delay_ms(2000);
    if(temperature & 0x800) temperature |= 0xF000; // Keep negative sign
    return (int)temperature;
}
