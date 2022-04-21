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

volatile int buttonPressed1 = 0;
volatile int buttonPressed2 = 0;



// char ostr[OSTR_SIZE];           // Buffer for creating strings

int main(void)
{
    /* Interrupt Initialization Code*/
    DDRC   &= ~(1 << PC0);
    DDRB   &= ~(1 << PB0);
    DDRC   &= ~(1 << PC1);
    DDRC   &= ~(1 << PC2);
    PORTB |= 0b00000001;
    PORTC |= 0b00000111;
    PCICR |= 0b00000011;
    PCIFR |= 0b00000011;
    
    PCMSK1 |= 0x07; //00000111
    PCMSK0  |= 1;
    sei();

    /* INITIALIZATION CODE */
    i2c_init(BDIV);
    ds1631_init();
    pcf8563Init();
    tsl2591_init();
   
    lcd_init();
    lcd_moveto(0, 0);
    char hello_char[6] = {'h', 'e', 'l', 'l', 'o', '\0'};
    // lcd_stringout(hello_char);

    /* TEMPERATURE SENSOR CONFIG */
    char warn_char[10] = {'t', 'e', 'm', 'p', '>', '2','5', (char)223, 'C', '\0'};
    // unsigned char buf[1] = {DS1631_CONFIG_1SHOT};
    ds1631SetTH(0); // 25 degress celsius
    _delay_ms(5);
    ds1631_start_convert();
    _delay_ms(5);
    // write to the high threshold register in temperature sensor
   

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

        // // output warning to LCD if temperature is above 4C
        // _delay_ms(500);
        // // check second digit
        // if ((int)rbuf[7] > 4)
        // {
        //     lcd_moveto(0, 11);
        //     _delay_ms(10);
        //     lcd_stringout(warn_char);
        // }
        // // check first digit if second is okay
        // else if ((int)rbuf[6] > 0)
        // {
        //     lcd_moveto(0, 11);
        //     _delay_ms(10);
        //     lcd_stringout(warn_char);
        // }
        // _delay_ms(50);

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
        if (lux < 3000)
        {
            doorJustOpened = 1;
            // time elapsed code
            unsigned char rbuf_time[19];
            getTime(rbuf_time, minAndSec);
            // compare current time with the stored time
            lcd_moveto(3, 0);
            int minPassed = minAndSec[0] - min_door_open;

            char minBuf[OSTR_SIZE]; // Buffer for creating strings
                                    // memset(ostr, 0, OSTR_SIZE);
            char minBuf2[OSTR_SIZE];
            // snprintf(ostr, OSTR_SIZE, "Light: %d%d", rbuf_low[0], rbuf_high[0]);

            snprintf(minBuf, OSTR_SIZE, "%d", minPassed);
            snprintf(minBuf2, OSTR_SIZE, "%d", minPassed - 1);
            if (minAndSec[1] >= sec_door_open)
            {

                char time_open_char[18] = {'D', 'o', 'o', 'r', ' ', 'o', 'p', 'e', 'n', ' ', 'f', 'o', 'r', ' ', minBuf[0], ' ', 'm', '\0'};
                lcd_stringout(time_open_char);
            }
            else
            {

                char time_open_char[18] = {'D', 'o', 'o', 'r', ' ', 'o', 'p', 'e', 'n', ' ', 'f', 'o', 'r', ' ', minBuf2[0], ' ', 'm', '\0'};
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
                lcd_moveto(3, 0);
                lcd_stringout("                  ");
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
        
        
        
        // TEMP INTERRUPT CODE
        if (aboveTempThresh){
            lcd_moveto(0,11);
            lcd_stringout("      ");
            lcd_stringout(warn_char);
        
        
        }
        
        
        if (buttonPressed1){
            buttonPressed1 = 0;
            lcd_moveto(3,0);
            lcd_stringout("button 1 pressed");
        }
           
        if (buttonPressed2){
            buttonPressed2 = 0;
            lcd_moveto(3,0);
            lcd_stringout("button 2 pressed");
        }
        
        unsigned char th_buf[2];
        ds1631ReadTH(th_buf);
        char string_buf[OSTR_SIZE];
        snprintf(string_buf,OSTR_SIZE,  "Yo: %d", th_buf[0]);
        lcd_moveto(3, 0);
        lcd_stringout(string_buf);
        
    }

    return 0; /* never reached */
}



ISR(PCINT0_vect){
  aboveTempThresh = 1;


}

ISR(PCINT1_vect)
{
    // Button code
    
    
    //if ((PINC & (1 << PC0))){
       
        
    
    //}
    
    
    if ((PINC & (1 << PC1))){
        buttonPressed1 = 1;
        
    
    }
    if ((PINC & (1 << PC2))){
        buttonPressed2 = 1;
     
    
    }
    
    
    
    
    // Some bit changed, see if it is PC0
    
    //aboveTempThresh = 1;
}


