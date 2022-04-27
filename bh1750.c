#include "bh1750.h"


void bh1750_init(void){
    // configure mode for light sensor
    uint8_t wbuf[1];
    wbuf[0] = BH1750_CONTINUOUS_HIGH_RES_MODE;
    i2c_io(BH1750_DEVICE_ADDR, wbuf, 1, NULL, 0, NULL, 0);
}

void bh1750_readLight(unsigned char* light){
    
    uint8_t rbuf[2];
    // read light data
    i2c_io(BH1750_DEVICE_ADDR, NULL, 0, NULL, 0, rbuf, 2);
    char ostr[OSTR_SIZE];  
    snprintf(ostr, OSTR_SIZE, "Lux: %d%d", rbuf[0], rbuf[1]);
    
    
    
    light[0] = ostr[0];
    light[1] = ostr[1];
    light[2] = ostr[2];
    light[3] = ostr[3];
    light[4] = ostr[4];
    light[5] = ostr[5];
    light[6] = ostr[6];
    light[7] = ostr[7];
    
}

