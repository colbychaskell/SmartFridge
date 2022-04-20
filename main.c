#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds1631.h"
#include "lcd.h"
#include "pcf8563.h"
#include "bh1750.h"
#include "tsl2591.h"

const unsigned char str1[] PROGMEM = ">> at328-5.c hi <<901234";

const unsigned char str2[] PROGMEM = ">> USC EE459L <<78901234";

volatile int aboveTempThresh = 0; // flag set by temp ISR

int min_door_open; // the minute when door was opened
int sec_door_open; // the second when door was opened

int doorJustOpened = 1; // flag to tell if the door just opened

// char ostr[OSTR_SIZE];           // Buffer for creating strings

int main(void)
{
    /* Interrupt Initialization Code*/
    // DDRC   &= ~(1 << PC0);
    PCICR |= 0b00000010;
    PCMSK1 |= (1 << PC0);
    sei();

    /* INITIALIZATION CODE */
    i2c_init(BDIV);
    ds1631_init();
    pcf8563Init();
    tsl2591_init();
    // bh1750_init();
    lcd_init();
    lcd_moveto(0, 0);
    char hello_char[6] = {'h', 'e', 'l', 'l', 'o', '\0'};
    // lcd_stringout(hello_char);

    /* TEMPERATURE SENSOR CONFIG */
    char warn_char[9] = {'t', 'e', 'm', 'p', '>', '4', (char)223, 'C', '\0'};
    unsigned char buf[1] = {DS1631_CONFIG_1SHOT};
    ds1631_set_config(buf);
    ds1631_start_convert();
    _delay_ms(5);
    // write to the high threshold register in temperature sensor
    ds1631SetTH();

    /* RTC CONFIG */
    // stopClock();
    setYear(22);
    setMonth(4);
    setDay(19);
    setHour(12);
    setMinut(57);
    setSecond(00);
    startClock();
    while (1)
    {
        /* TEMP SENSOR CODE */
        unsigned char rbuf[8];
        char temp_char[11];
        _delay_ms(50);
        ds1631_read_temp(rbuf);
        _delay_ms(50);
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

        // output temperature to LCD
        lcd_moveto(0, 0);
        _delay_ms(10);
        lcd_stringout(temp_char);

        // output warning to LCD if temperature is above 4C
        _delay_ms(500);
        // check second digit
        if ((int)rbuf[7] > 4)
        {
            lcd_moveto(0, 11);
            _delay_ms(10);
            lcd_stringout(warn_char);
        }
        // check first digit if second is okay
        else if ((int)rbuf[6] > 0)
        {
            lcd_moveto(0, 11);
            _delay_ms(10);
            lcd_stringout(warn_char);
        }
        _delay_ms(50);

        /* TSL2591 */

        unsigned char rbuf_light[14];
        char light_char[14];
        // memset(light_char, 0, 14);
        _delay_ms(50);
        float lux = readLight(rbuf_light);
        _delay_ms(50);
        light_char[0] = (char)rbuf_light[0];
        light_char[1] = (char)rbuf_light[1];
        light_char[2] = (char)rbuf_light[2];
        light_char[3] = (char)rbuf_light[3];
        light_char[4] = (char)rbuf_light[4];
        light_char[5] = (char)rbuf_light[5];
        light_char[6] = (char)rbuf_light[6];
        light_char[7] = (char)rbuf_light[7];
        light_char[8] = (char)rbuf_light[8];
        light_char[9] = (char)rbuf_light[9];
        light_char[10] = (char)rbuf_light[10];
        light_char[11] = (char)rbuf_light[11];
        light_char[12] = (char)rbuf_light[12];
        light_char[13] = (char)rbuf_light[13];
        // light_char[14] = (char)rbuf_light[14];
        // light_char[15] = (char)rbuf_light[15];

        // output light to LCD
        lcd_moveto(2, 6);
        lcd_stringout("          ");
        lcd_moveto(2, 0);
        lcd_stringout(light_char);
        int minAndSec[2];
        if (lux == 0)
        {
            doorJustOpened = 1; 
            // time elapsed code
            unsigned char rbuf_time[19];
            getTime(rbuf_time, minAndSec);
            // compare current time with the stored time
            lcd_moveto(3, 0);
            int minPassed = minAndSec[0] - min_door_open;
            int secPassed = minAndSec[1] - sec_door_open;
            char minBuf[OSTR_SIZE]; // Buffer for creating strings
                                    // memset(ostr, 0, OSTR_SIZE);
            char minBuf2[OSTR_SIZE];
            // snprintf(ostr, OSTR_SIZE, "Light: %d%d", rbuf_low[0], rbuf_high[0]);

            snprintf(minBuf, OSTR_SIZE, "%d", minPassed);
            snprintf(minBuf2, OSTR_SIZE, "%d", minPassed - 1);
            if (minAndSec[1] >= sec_door_open)
            {

                char time_open_char[20] = {'d', 'o', 'o', 'r', ' ', 'o', 'p', 'e', 'n', ' ', 'f', 'o', 'r', ' ', minBuf[0], '!', '\0'};
                lcd_stringout(time_open_char);
            }
            else
            {

                char time_open_char[20] = {'d', 'o', 'o', 'r', ' ', 'o', 'p', 'e', 'n', ' ', 'f', 'o', 'r', ' ', minBuf2[0], '!', '\0'};
                lcd_stringout(time_open_char);
            }

            _delay_ms(50);

            // // output door open/closed code
            // lcd_moveto(3, 0);
            // lcd_stringout("            ");
            // lcd_moveto(3, 0);
            // lcd_stringout("Door Closed!");
        }
        else
        {
            if (doorJustOpened == 1)
            {
                doorJustOpened = 0;
                unsigned char rbuf_time[19];
                getTime(rbuf_time, minAndSec);
                min_door_open = minAndSec[0];
                sec_door_open = minAndSec[1];
            }

          
            // lcd_moveto(3, 0);
            // lcd_stringout("            ");
            // lcd_moveto(3, 0);
            // lcd_stringout("Door Open!");
        }
        _delay_ms(50);

        // // /* BH1750 LIGHT SENSOR CODE */
        // // read lux
        // unsigned char rbuf_light[9];
        // bh1750_readLight(rbuf_light);
        // // output lux to LCD
        // char temp_char2[11];
        // temp_char2[0] = (char)rbuf_light[0];
        // temp_char2[1] = (char)rbuf_light[1];
        // temp_char2[2] = (char)rbuf_light[2];
        // temp_char2[3] = (char)rbuf_light[3];
        // temp_char2[4] = (char)rbuf_light[4];
        // temp_char2[5] = (char)rbuf_light[5];
        // temp_char2[6] = (char)rbuf_light[6];
        // temp_char2[7] = (char)rbuf_light[7];
        // temp_char2[8] = '\0';
        // lcd_moveto(3, 0);
        // _delay_ms(10);
        // lcd_stringout(temp_char2);

        // _delay_ms(50);

        /* REAL TIME CLOCK CODE */
        // read time
        unsigned char rbuf_time[19];

        getTime(rbuf_time, minAndSec);

        _delay_ms(50);
        // output time to LCD
        char time_char[20];
        time_char[0] = (char)rbuf_time[0];
        time_char[1] = (char)rbuf_time[1];
        time_char[2] = (char)rbuf_time[2];
        time_char[3] = (char)rbuf_time[3];
        time_char[4] = (char)rbuf_time[4];
        time_char[5] = (char)rbuf_time[5];
        time_char[6] = (char)rbuf_time[6];
        time_char[7] = (char)rbuf_time[7];
        time_char[8] = (char)rbuf_time[8];
        time_char[9] = (char)rbuf_time[9];
        time_char[10] = (char)rbuf_time[10];
        time_char[11] = (char)rbuf_time[11];
        time_char[12] = (char)rbuf_time[12];
        time_char[13] = (char)rbuf_time[13];
        time_char[14] = (char)rbuf_time[14];
        time_char[15] = (char)rbuf_time[15];
        time_char[16] = (char)rbuf_time[16];
        time_char[17] = (char)rbuf_time[17];
        time_char[18] = (char)rbuf_time[18];
        time_char[19] = '\0';
        lcd_moveto(1, 0);
        _delay_ms(10);
        lcd_stringout(time_char);
        _delay_ms(100);
    }

    return 0; /* never reached */
}

ISR(PCINT1_vect)
{
    // Some bit changed, see if it is PC0
    if ((PORTC & (1 << PC0)) == 1)
    {
        aboveTempThresh = 1;
    }
}
