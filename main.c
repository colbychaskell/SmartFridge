#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds1631.h"
#include "lcd.h"

const unsigned char str1[] PROGMEM = ">> at328-5.c hi <<901234";

const unsigned char str2[] PROGMEM = ">> USC EE459L <<78901234";

volatile int aboveTempThresh = 0; // flag set by temp ISR

// char ostr[OSTR_SIZE];           // Buffer for creating strings



int main(void)
{
    /* Interrupt Initialization Code*/
    // DDRC   &= ~(1 << PC0);
	PCICR  |=  0b00000010;
    PCMSK1  |=  (1 << PC0);
	sei();
    
    /* Initialization Code */
    i2c_init(BDIV);
    ds1631_init();
    lcd_init();
    lcd_moveto(0, 0);
    char hello_char[6] = {'h', 'e', 'l', 'l', 'o', '\0'};
    char warn_char[9] = {'t', 'e', 'm', 'p', '>', '4', (char)223, 'C', '\0'};
    lcd_stringout(hello_char);
    unsigned char buf[1] = {DS1631_CONFIG_1SHOT};
    ds1631_set_config(buf);
    ds1631_start_convert();
    _delay_ms(5);

    // write to the high threshold register in temperature sensor
    
    ds1631SetTH();

    while (1)
    {
        // unsigned char rbuf[8];
        // char temp_char[11];
        // ds1631_start_convert();
        unsigned char rbuf[8];
        char temp_char[11];
        ds1631_read_temp(rbuf);
        _delay_ms(500);

        temp_char[0] = (char)rbuf[0];
        temp_char[1] = (char)rbuf[1];
        temp_char[2] = (char)rbuf[2];
        temp_char[3] = (char)rbuf[3];
        temp_char[4] = (char)rbuf[4];
        temp_char[5] = (char)rbuf[5];
        temp_char[6] = (char)rbuf[6];
        temp_char[7] = (char)rbuf[7];
        temp_char[8] = (char)223;
        temp_char[9] = 'C';
        temp_char[10] = '\0';
        // temp_char[7] = (char)rbuf[7];
        /* LCD Use */

        lcd_moveto(1, 0);
        _delay_ms(500);
        lcd_stringout(temp_char);

        if (aboveTempThresh)
        {

            _delay_ms(500);
            // check second digit
            if ((int)rbuf[7] > 4)
            {
                lcd_moveto(2, 0);
                _delay_ms(10);
                lcd_stringout(warn_char);
            }
            else if ((int)rbuf[6] > 0)
            {
                lcd_moveto(2, 0);
                _delay_ms(10);
                lcd_stringout(warn_char);
            }
        }
    }

    return 0; /* never reached */
}

ISR(PCINT1_vect)
{
    // Some bit changed, see if it is PC0
    if ((PORTC & (1 << PC0)) == 0)
    {
        aboveTempThresh = 1;
    }
}
