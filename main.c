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
volatile char tempThresChanged = 0;
volatile int buttonPressed1 = 0;
volatile int buttonPressed2 = 0;
volatile int alarmFlag = 0;
volatile char alarmChanged = 0;

volatile int menuPress = 0;


float LIGHT_THRESHOLD = 6000;

    enum {
        Door_Open_State,
        Door_Closed_State,
        Food_Selection_State, 
        Time_Selection_State,
        Food_Display_State
    };

void output_temp_lcd()
{
    char temp_string[6];
    lcd_moveto(0, 0);
    _delay_ms(10);
    ds1631_formatted_temp(temp_string);
    lcd_stringout(temp_string);
}

void set_rtc_time()
{
    // stopClock();
    setYear(22);
    setMonth(4);
    setDay(22);
    setHour(10);      
    setMinute(05);
    setSecond(00);   
    startClock();
}

void light_output_debug(float lux)
{
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

float get_light_value(void)
{
    unsigned char rbuf_light[14];
    char light_char[14];
    return readLight(rbuf_light);
}

void temp_threshold_debug()
{
    /* DEBUG CODE FOR TEMP TEMP_THRESHOLD*/
    unsigned char th_buf[2];
    unsigned char th_buf_low[2];
    ds1631ReadTH(th_buf, th_buf_low);
    char string_buf[OSTR_SIZE];
    snprintf(string_buf, OSTR_SIZE, "LOW TH:%d  HIGH TH:%d", th_buf_low[0], th_buf[0]);
    lcd_moveto(3, 0);
    lcd_stringout(string_buf);
}

void button_debug()
{
    /* DEBUG CODE FOR BUTTON PRESS ISR*/
    if (buttonPressed1)
    {
        buttonPressed1 = 0;
        lcd_moveto(3, 0);
        lcd_stringout("button 1 pressed");
    }

    if (buttonPressed2)
    {
        buttonPressed2 = 0;
        lcd_moveto(3, 0);
        lcd_stringout("button 2 pressed");
    }
}

void update_current_time(int *current_time)
{
    unsigned char time_string[6];
    getTime(time_string, current_time); // Read time from RTC

    lcd_moveto(0, 15);
    lcd_stringout(time_string);
}

int main(void)
{
    int current_time[2];
    int total_time_open = 0;
    int door_open_time[2]; 

    char flash_count = 0;

    unsigned char state = Door_Open_State;

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


    unsigned char temp[2];
    ds1631_read_temp(temp);

    int TEMP_THRESHOLD = (int)temp[0] + 2;

    /* TEMPERATURE SENSOR CONFIG */
    ds1631SetTH(TEMP_THRESHOLD, TEMP_THRESHOLD); // 25 degress celsius
    ds1631_start_convert();

    /* RTC CONFIG */
    set_rtc_time();
    update_current_time(door_open_time);

    /* Initialize Temp Threshold Flag */
    if(temp[0] <= TEMP_THRESHOLD) {
        aboveTempThresh = 0;
    }
    else {
        aboveTempThresh = 1;
    }

    /* Main Loop */
    while (1)
    {
        /* STATE MACHINE */
        if (state == Door_Open_State)
        {

            /* TEMP SENSOR CODE */
            output_temp_lcd();

            /* RTC CODE */
            update_current_time(current_time); // Update time displayed and stored in current_time

            lcd_moveto(1,0);
            lcd_stringout("PRESS BLACK BUTTON");
            lcd_moveto(2,0);
            lcd_stringout("FOR FOOD TRACKER");

            /* Alarm Code*/
            if (alarmFlag)
            {
                PORTC |= (1<<PC3);
                lcd_moveto(3,0);
                if(!aboveTempThresh) {
                    lcd_stringout("Alarm: ARMED  ");
                }
            }
            else
            {
                PORTC &= ~(1<<PC3);
                lcd_moveto(3,0);
                if(!aboveTempThresh) {
                    lcd_stringout("Alarm: UNARMED");
                }
            }

            /* TEMP ALARM */
            if (aboveTempThresh) // Temp low to high
            {
                flash_count++;
                if(flash_count < 4) {
                    lcd_moveto(3, 0);
                    lcd_stringout("           TEMP HIGH");
                }
                else if(flash_count < 8) {
                    lcd_moveto(3, 0);
                    lcd_stringout("                    ");
                }
                else {
                    flash_count = 0;
                }
                
            }
            else { // Temp high to low
                if(tempThresChanged) {
                    tempThresChanged = 0;
                    lcd_moveto(3, 11);
                    lcd_stringout("         ");
                }
            }

            /* State Transitions */
            if (get_light_value() < LIGHT_THRESHOLD) // If door is closed
            {
                state = Door_Closed_State;

                // TURN OFF ALARM IF ON
                PORTC &= ~(1 << PC3);

                //UPDATE DISPLAY
                lcd_moveto(1, 0);
                lcd_stringout("                    ");

                // compare current time with the stored time
                int mins_last_opened = current_time[0] - door_open_time[0];
                
                if (current_time[1] >= door_open_time[1])
                {
                    total_time_open += mins_last_opened;
                }
                else {
                    total_time_open += mins_last_opened - 1;
                }

                // Display time door has been open on LCD
                lcd_moveto(1,0);
                lcd_stringout("Door Open Today:  ");
                lcd_moveto(2, 0);
                char time_open_string[18];
                snprintf(time_open_string, 18, "%02dmins              ", total_time_open);
                lcd_stringout(time_open_string);
            }
        }
        if (state == Door_Closed_State)
        {

            /* TEMP SENSOR CODE */
            output_temp_lcd();

            /* Alarm Code*/
            if (alarmFlag)
            {
                lcd_moveto(3, 0);
                lcd_stringout("Alarm: ARMED  ");
            }
            else
            {
                lcd_moveto(3, 0);
                lcd_stringout("Alarm: UNARMED");
            }

            /* State Transitions */
            if (get_light_value() > LIGHT_THRESHOLD)
            {
                state = Door_Open_State;

                // Record time that door is opened
                unsigned char rbuf_time[19]; // Not rlly needed
                getTime(rbuf_time, current_time);
                door_open_time[0] = current_time[0];
                door_open_time[1] = current_time[1];

                lcd_moveto(2,0);
                lcd_stringout("                 ");
            }
        }
    }
    return 0; /* never reached */
}

ISR(PCINT0_vect)
{
    tempThresChanged = 1;
    if (aboveTempThresh)
    {
        aboveTempThresh = 0;
    }
    else
    {
        aboveTempThresh = 1;
    }
}

ISR(PCINT1_vect)
{
    if ((PINC & (1 << PC1))) // If alarm button is pressed
    {
        alarmChanged = 1;
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

    if ((PINC & (1 << PC2))) // If button 2 is pressed
    {
        buttonPressed2 = 1;
    }
}

