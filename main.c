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

volatile int aboveTempThresh = 0; // flag set by temp ISR

int min_door_open; // the minute when door was opened
int sec_door_open; // the second when door was opened

int doorJustOpened = 1; // flag to tell if the door just opened

volatile int buttonPressed1 = 0;
volatile int buttonPressed2 = 0;
volatile int alarmFlag = 0;

char THRESHOLD = 28;

void output_temp_lcd() {
    char temp_string[12];
    lcd_moveto(0, 0);
    _delay_ms(10);
    ds1631_formatted_temp(temp_string);
    lcd_stringout(temp_string);
}

void config_rtc() {
    // stopClock();
    setYear(22);
    setMonth(4);
    setDay(19);
    setHour(12);
    setMinute(57);
    setSecond(00);
    startClock();
}

void light_output_debug(float lux) {
    unsigned char rbuf_light[14];
    char light_char[14];
    lux = readLight(rbuf_light);

    // /* DEBUG FOR LIGHT SENSOR CODE */
    snprintf(light_char, 15, "%s", rbuf_light);

    // output light to LCD
    lcd_moveto(2, 6);
    lcd_stringout("          ");
    lcd_moveto(2, 0);
    lcd_stringout(light_char);
}

float light_output(void) {
    unsigned char rbuf_light[14];
    char light_char[14];
    return readLight(rbuf_light);
}

void temp_threshold_debug() {
    // // /* DEBUG CODE FOR TEMP THRESHOLD*/
    // // unsigned char th_buf[2];
    // // unsigned char th_buf_low[2];
    // // ds1631ReadTH(th_buf, th_buf_low);
    // // char string_buf[OSTR_SIZE];
    // // snprintf(string_buf,OSTR_SIZE,  "LOW TH:%d  HIGH TH:%d", th_buf_low[0], th_buf[0]);
    // // lcd_moveto(3, 0);
    // // lcd_stringout(string_buf);
}

void button_debug() {
    // /* DEBUG CODE FOR BUTTON PRESS ISR*/
    // // if (buttonPressed1)
    // // {
    // //     buttonPressed1 = 0;
    // //     lcd_moveto(3, 0);
    // //     lcd_stringout("button 1 pressed");
    // // }

    // // if (buttonPressed2)
    // // {
    // //     buttonPressed2 = 0;
    // //     lcd_moveto(3, 0);
    // //     lcd_stringout("button 2 pressed");
    // // }
}

int main(void)
{
    char STATE_TEST = 1;
    char doorFlag = 0;


    /* Interrupt Initialization Code*/
    DDRC &= ~(1 << PC0);
    DDRB &= ~(1 << PB0);
    DDRC |= (1 << PC3);
    DDRC &= ~(1 << PC1);
    DDRC &= ~(1 << PC2);
    PORTB |= 0b00000001;
    PORTC |= 0b00000111;
    PCICR |= 0b00000011;
    PCIFR |= 0b00000011;

    PCMSK1 |= 0x07; // 00000111
    PCMSK0 |= 1;
    sei();

    /* INITIALIZATION CODE */
    i2c_init(BDIV);
    ds1631_init();
    pcf8563Init();
    tsl2591_init();
    lcd_init();

    /* TEMPERATURE SENSOR CONFIG */
    char warn_char[10] = {'t', 'e', 'm', 'p', '>', '2', '5', (char)223, 'C', '\0'};
    ds1631SetTH(THRESHOLD, THRESHOLD); // 25 degress celsius
    
    /* RTC CONFIG */
    config_rtc();

    /* Main Loop */
    //ds1631_start_convert();
    while (1)
    {



        /* STATE MACHINE */
        if(STATE_TEST) {
            /* TEMP SENSOR CODE */
            output_temp_lcd();
            //light_output_debug();

            /* RTC CODE */
            int minAndSec[2];
            unsigned char rbuf_time[19];
            getTime(rbuf_time, minAndSec); // Read time from RTC
            _delay_ms(50);
            // output time to LCD
            char time_char[20];

            snprintf(time_char, 20, "%s", rbuf_time);

            lcd_moveto(1, 0);
            _delay_ms(10);
            lcd_stringout(time_char);
            _delay_ms(100);
            
            float lux = light_output();

            if (lux < 3000) // If door is closed
            {
                doorFlag = 1;
                
                if (alarmFlag)
                {
                    lcd_moveto(2, 0);
                    lcd_stringout("Alarm is on ");
                }
                else
                {
                    lcd_moveto(2, 0);
                    lcd_stringout("Alarm is off ");
                }

                doorJustOpened = 1; // reset flag
                // time elapsed code
                unsigned char rbuf_time[19];
                getTime(rbuf_time, minAndSec);
                // compare current time with the stored time
                lcd_moveto(3, 0);
                int minPassed = minAndSec[0] - min_door_open;

                char time_open_char[18];
                if (minAndSec[1] >= sec_door_open)
                {
                    snprintf(time_open_char, 18, "Door open for %d m", minPassed);
                    lcd_stringout(time_open_char);
                }
                else
                {
                    snprintf(time_open_char, 18, "Door open for %d m", (minPassed - 1));
                    lcd_stringout(time_open_char);
                }

                _delay_ms(50);

                // // output door open/closed code
                // lcd_moveto(3, 0);
                // lcd_stringout("            ");
                // lcd_moveto(3, 0);
                // lcd_stringout("Door Closed!");
            }
            else // Door open
            {
                if (alarmFlag)
                {
                    lcd_moveto(2, 0);
                    lcd_stringout("Alarm is on ");
                    // sound buzzer
                    PORTC |= (1 << PC3);
                }
                else
                {
                    lcd_moveto(2, 0);
                    lcd_stringout("Alarm is off ");
                    PORTC &= ~(1 << PC3);
                }
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
        }
        else {
            // TEMP INTERRUPT CODE
            if (aboveTempThresh)
            {
                lcd_moveto(0, 11);
                lcd_stringout("      ");
                lcd_moveto(0, 11);
                lcd_stringout(warn_char);
            }
        }

        _delay_ms(50);

    }

    return 0; /* never reached */
}

ISR(PCINT0_vect)
{
    if (aboveTempThresh) {
        aboveTempThresh = 0;
    }
    else {
        aboveTempThresh = 1;
    }
}

ISR(PCINT1_vect)
{
    // Button code

    // if ((PINC & (1 << PC0))){

    //}

    if ((PINC & (1 << PC1)))
    {
        if (alarmFlag)
        {
            alarmFlag = 0;
        }
        else
        {
            alarmFlag = 1;
        }

        buttonPressed1 = 1;
    }
    if ((PINC & (1 << PC2)))
    {
        buttonPressed2 = 1;
    }

    // Some bit changed, see if it is PC0

    // aboveTempThresh = 1;
}

// // // output warning to LCD if temperature is above 4C
// // _delay_ms(500);
// // // check second digit
// // if ((int)temp[7] > 4)
// // {
// //     lcd_moveto(0, 11);
// //     _delay_ms(10);
// //     lcd_stringout(warn_char);
// // }
// // // check first digit if second is okay
// // else if ((int)temp[6] > 0)
// // {
// //     lcd_moveto(0, 11);
// //     _delay_ms(10);
// //     lcd_stringout(warn_char);
// // }
// // _delay_ms(50);
