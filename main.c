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
    unsigned char temp_char[2]; 
    temp_char[0] = (unsigned char)temp;
    temp_char[1] = "yo";
   
    // lcd 
    lcd_init();                 // Initialize the LCD display
    
    lcd_moveto(0, 0);
    _delay_ms(500);        
    lcd_moveto(0, 2);
    _delay_ms(500);        
    lcd_moveto(1, 0);
    _delay_ms(500);        
    lcd_moveto(0, 0);
     _delay_ms(500); 
    lcd_stringout(str1);
    lcd_stringout("temp: ");        // Print string on line 1
    lcd_stringout(str2);
    //lcd_moveto(0, 2);
    //lcd_stringout(str2);        // Print string on line 2

    while (1) {                 // Loop forever
    }

    

    return 0;   /* never reached */
    
}
