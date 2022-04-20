#include "tsl2591.h"

void enable(void)
{

    // Enable the device by setting the control bit to 0x01
    uint8_t wbuf[1];
    wbuf[0] = TSL2591_COMMAND_BIT | 0x00;
    uint8_t data[1];
    data[0] = TSL2591_ENABLE_POWERON | TSL2591_ENABLE_AEN | TSL2591_ENABLE_AIEN |
              TSL2591_ENABLE_NPIEN;
    i2c_io(TSL2591_ADDR, wbuf, 1, data, 1, NULL, 0);
}

void tsl2591_init(void)
{
    enable();
}

void disable(void)
{

    // Disable the device by setting the control bit to 0x00
    uint8_t wbuf[1];
    wbuf[0] = TSL2591_COMMAND_BIT | 0x00;
    uint8_t data[1];
    data[0] = TSL2591_ENABLE_POWEROFF;
    i2c_io(TSL2591_ADDR, wbuf, 1, data, 1, NULL, 0);
}

float readLight(unsigned char *rbuf)
{

    // unsigned char wbuf[1];
    // unsigned char wbuf2[1];
    // unsigned char wbuf3[1];
    // wbuf[0] = TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN0_LOW;
    // wbuf2[0] = TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN1_LOW;

    // unsigned char rbuf_low[1];
    // unsigned char rbuf_high[1];
    // i2c_io(TSL2591_ADDR, wbuf, 1, NULL, 0, rbuf_low, 1);   // x
    // i2c_io(TSL2591_ADDR, wbuf2, 1, NULL, 0, rbuf_high, 1); // y

    unsigned char wbuf[1];
    unsigned char wbuf2[1];
    unsigned char wbuf3[1];
    wbuf[0] = TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN0_LOW;
    wbuf2[0] = TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN1_LOW;
    uint8_t rbuf0_low[2];
    uint8_t rbuf1_low[2];
    i2c_io(TSL2591_ADDR, wbuf, 1, NULL, 0, rbuf0_low, 2);  // y
    i2c_io(TSL2591_ADDR, wbuf2, 1, NULL, 0, rbuf1_low, 2); // x
    uint32_t x;
    x = rbuf1_low[0];
    x <<= 8;
    x |= rbuf1_low[1];
    x <<= 16;
    uint16_t y;
    y = rbuf0_low[0];
    y <<= 8;
    y |= rbuf0_low[1];
    x |= y;
    uint16_t ir, full;
    ir = x >> 16;
    full = x & 0xFFFF;
    float lux = calculateLux(full, ir);
    uint32_t lux_int = (uint32_t)lux;

    char ostr[OSTR_SIZE]; // Buffer for creating strings
                          // memset(ostr, 0, OSTR_SIZE);

    //snprintf(ostr, OSTR_SIZE, "Light: %d%d", rbuf_low[0], rbuf_high[0]);

    
    snprintf(ostr, OSTR_SIZE, "Light: %ld", lux_int);

    rbuf[0] = ostr[0];
    rbuf[1] = ostr[1];
    rbuf[2] = ostr[2];
    rbuf[3] = ostr[3];
    rbuf[4] = ostr[4];
    rbuf[5] = ostr[5];
    rbuf[6] = ostr[6];
    rbuf[7] = ostr[7];
    rbuf[8] = ostr[8];
    rbuf[9] = ostr[9];
    rbuf[10] = ostr[10];
    rbuf[11] = ostr[11];
    rbuf[12] = ostr[12];
    rbuf[13] = '\0';
    
    return lux_int;
}

float calculateLux(uint16_t ch0, uint16_t ch1)
{
    float atime, again;
    float cpl, lux1, lux2, lux;

    atime = 100.0F;
    again = 1.0F;
    cpl = (atime * again) / TSL2591_LUX_DF;
    lux1 = (ch0 - (TSL2591_LUX_COEFB * ch1)) / cpl;
    lux2 = ((TSL2591_LUX_COEFC * ch0) - (TSL2591_LUX_COEFC * ch1)) / cpl;
    if (lux1 > lux2)
        return lux1;
    else
        return lux2;
}
