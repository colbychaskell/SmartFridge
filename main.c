/*********************************************************************
*       at328-7.c - Demonstrate reading and writing to an I2C EEPROM
*
*       This program store some information in a 24LC256 (32k x 8)
*       I2C EEPROM and then read it back and reports the results out
*       the RS-232 interface.
*       If the button on PC1 is pressed it writes and then reads.
*       If the button is not pressed it only does the reading part.
*

* Revision History
* Date     Author      Description
* 01/28/08 A. Weber    First Release for MC68HC908JL16
* 02/12/08 A. Weber    Clean up code a bit
* 03/03/08 A. Weber    More code cleanups
* 04/22/08 A. Weber    Added "one" variable to make warning go away
* 04/14/11 A. Weber    Adapted for ATmega168
* 05/12/12 A. Weber    Modified for new I2C routines
* 11/18/13 A. Weber    Renamed for ATmega328P
* 04/01/15 A. Weber    Replaced sprintf's with snprintf's
*********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>


#define FOSC 9830400 // Clock frequency = Oscillator freq.
#define BDIV (FOSC / 100000 - 16) / 2 + 1

uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an,
               uint8_t *wp, uint16_t wn, uint8_t *rp, uint16_t rn)
{
    uint8_t status, send_stop, wrote, start_stat;

    status = 0;
    wrote = 0;
    send_stop = 0;

    if (an > 0 || wn > 0)
    {
        wrote = 1;
        send_stop = 1;

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA); // Send start condition
        while (!(TWCR & (1 << TWINT)))
            ; // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x08) // Check that START was sent OK
            return (status);

        TWDR = device_addr & 0xfe;         // Load device address and R/W = 0;
        TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
        while (!(TWCR & (1 << TWINT))){
            
        }; // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x18)
        {                       // Check that SLA+W was sent OK
            if (status == 0x20) // Check for NAK
                goto nakstop;   // Send STOP condition
            return (status);    // Otherwise just return the status
        }

        // Write "an" data bytes to the slave device
        while (an-- > 0)
        {
            TWDR = *ap++;                      // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT)))
                ; // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28)
            {                       // Check that data was sent OK
                if (status == 0x30) // Check for NAK
                    goto nakstop;   // Send STOP condition
                return (status);    // Otherwise just return the status
            }
        }

        // Write "wn" data bytes to the slave device
        while (wn-- > 0)
        {
            TWDR = *wp++;                      // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT)))
                ; // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28)
            {                       // Check that data was sent OK
                if (status == 0x30) // Check for NAK
                    goto nakstop;   // Send STOP condition
                return (status);    // Otherwise just return the status
            }
        }

        status = 0; // Set status value to successful
    }

    if (rn > 0)
    {
        send_stop = 1;

        // Set the status value to check for depending on whether this is a
        // START or repeated START
        start_stat = (wrote) ? 0x10 : 0x08;

        // Put TWI into Master Receive mode by sending a START, which could
        // be a repeated START condition if we just finished writing.
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
        // Send start (or repeated start) condition
        while (!(TWCR & (1 << TWINT)))
            ; // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != start_stat) // Check that START or repeated START sent OK
            return (status);

        TWDR = device_addr | 0x01;         // Load device address and R/W = 1;
        TWCR = (1 << TWINT) | (1 << TWEN); // Send address+r
        while (!(TWCR & (1 << TWINT)))
            ; // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x40)
        {                       // Check that SLA+R was sent OK
            if (status == 0x48) // Check for NAK
                goto nakstop;
            return (status);
        }

        // Read all but the last of n bytes from the slave device in this loop
        rn--;
        while (rn-- > 0)
        {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Read byte and send ACK
            while (!(TWCR & (1 << TWINT)))
                ; // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x50) // Check that data received OK
                return (status);
            *rp++ = TWDR; // Read the data
        }

        // Read the last byte
        TWCR = (1 << TWINT) | (1 << TWEN); // Read last byte with NOT ACK sent
        while (!(TWCR & (1 << TWINT)))
            ; // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x58) // Check that data received OK
            return (status);
        *rp++ = TWDR; // Read the data

        status = 0; // Set status value to successful
    }

nakstop: // Come here to send STOP after a NAK
    if (send_stop)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // Send STOP condition

    return (status);
}

/*
  i2c_init - Initialize the I2C port
*/
void i2c_init(uint8_t bdiv)
{
    TWSR = 0;    // Set prescalar for 1
    TWBR = bdiv; // Set bit rate register
}

/*
    Read temp data (2 bytes) into buf
*/
void read_temp(unsigned char* buf) {
    unsigned char status;
    unsigned char device_addr;
    unsigned char reg_addr;
    unsigned char data;

    /* Temp Sensor */
    device_addr = 0x90;
    reg_addr = 0xAC;
    data = 0x02;
    status = i2c_io(device_addr, &reg_addr, 1, &data, 1, NULL, 0);
    // read from "device id register'
    //reg_addr = 0xEE;
    //status = i2c_io(device_addr, &reg_addr, 1, NULL, 0, NULL, 0);
    reg_addr = 0xAA;
    while(1) {
        status = i2c_io(device_addr, &reg_addr, 1, NULL, 0, buf, 2);
    }
    //_delay_ms(5);
    status = i2c_io(device_addr, &reg_addr, 1, NULL, 0, buf, 2);
    // _delay_ms(2000);
}

int main(void)
{
    unsigned char temp_data[2];
    i2c_init(BDIV); // Initialize the I2C port

    read_temp(temp_data);


}
