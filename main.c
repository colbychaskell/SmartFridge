#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds1631.h"
#include "lcd.h"


const unsigned char str1[] PROGMEM = ">> at328-5.c hi <<901234";

const unsigned char str2[] PROGMEM = ">> USC EE459L <<78901234";



int main(void)
{
    /* Initialization Code */
    ds1631_init();
    lcd_init();
    
    
    unsigned char buf[1] = {DS1631_CONFIG_1SHOT};
    //ds1631_set_config(buf);
    //ds1631_start_convert();
    //int temp = ds1631_read_temp();
    char temp_char[16] = {'h', 'e', 'l', 'l', 'o','\0'};
   
    
    //char temperature2[16];
    //temp_char[0] = (unsigned char)temp;
    //temp_char[1] = "yo";

    //itoa(temp, temperature2, 2);

    /* LCD Use */
    lcd_init(); // Initialize the LCD display

    lcd_moveto(0,0);
    _delay_ms(500);

    lcd_stringout(temp_char);


        while (1)
        {

    }
    return 0;   /* never reached */
    
}
