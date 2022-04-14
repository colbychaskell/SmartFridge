#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds1631.h"
#include "lcd.h"

const unsigned char str1[] PROGMEM = ">> at328-5.c hi <<901234";

const unsigned char str2[] PROGMEM = ">> USC EE459L <<78901234";

//char ostr[OSTR_SIZE];           // Buffer for creating strings

int main(void)
{
    /* Initialization Code */
    i2c_init(BDIV);
    ds1631_init();
    
    unsigned char buf[1] = {DS1631_CONFIG_1SHOT};
    ds1631_set_config(buf);
    ds1631_start_convert();
    unsigned char rbuf[8];
    //int temp = ds1631_read_temp(rbuf);
    ds1631_read_temp(rbuf);
    _delay_ms(500);
    
    char hello_char[6] = {'h', 'e', 'l', 'l', 'o', '\0'};
    char temp_char[9];
    temp_char[0] = (char)rbuf[0];
    temp_char[1] = (char)rbuf[1];
    temp_char[2] = (char)rbuf[2];
    temp_char[3] = (char)rbuf[3];
    temp_char[4] = (char)rbuf[4];
    temp_char[5] = (char)rbuf[5];
    temp_char[6] = (char)rbuf[6];
    temp_char[7] = (char)rbuf[7];
    temp_char[8] = '\0';
    //temp_char[7] = (char)rbuf[7];
    /* LCD Use */
    lcd_init();
    lcd_moveto(0, 0);
    //_delay_ms(500);
    lcd_stringout(hello_char);
    //_delay_ms(500);
    lcd_moveto(1,0);
    _delay_ms(500);
    lcd_stringout(temp_char);
    
    while (1)
    {
    }
    return 0; /* never reached */
}
