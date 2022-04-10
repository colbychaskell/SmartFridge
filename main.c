#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "ds1631.h"
#include "lcd.h"


const unsigned char str1[] PROGMEM = ">> at328-5.c hi <<901234";

const unsigned char str2[] PROGMEM = ">> USC EE459L <<78901234";


int main(void)
{
    ds1631_init();
    unsigned char buf[1] = {DS1631_CONFIG_1SHOT};
    ds1631_set_config(buf);
    ds1631_start_convert();
    int temp = ds1631_read_temp();
    
    // lcd 
    lcd_init();                 // Initialize the LCD display

    lcd_moveto(0, 0);
    lcd_stringout(str1);        // Print string on line 1

    lcd_moveto(1, 0);
    lcd_stringout(str2);        // Print string on line 2

    while (1) {                 // Loop forever
    }

    

    return 0;   /* never reached */
    
}
