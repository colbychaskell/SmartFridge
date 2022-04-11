#include "lcd.h"

/*
  lcd_init - Do various things to force a initialization of the LCD
  display by instructions, and then set up the display parameters and
  turn the display on.
*/
void lcd_init(void)
{
    DDRD |= LCD_Data_D;         // Set PORTD bits 0-8 for output
    DDRB |= LCD_Bits;

    PORTB &= ~LCD_RS;           // Clear RS (PB2) for command write

    _delay_ms(100);              // Delay at least 40ms

    lcd_writebyte(0x30);        // Send 00110000 to data pins, set for 8-bit interface (Function set)
    _delay_ms(5);               // Delay at least 39usec

    lcd_writebyte(0x30);        // Send 00110000 to data pins, set for 8-bit interface (Function set)
    _delay_us(1);             // Delay at least 37usec

    
    lcd_writebyte(0x0f);        // Send 00001111 to data pins, set for 8-bit interface (Display ON/OFF control)
    _delay_ms(2);               // Delay at least 37usec
    
    lcd_writebyte(0x01);        // Send 00000001 to data pins, set for 8-bit interface (Display Clear) (DB4=1/0)
    _delay_ms(5);               // Delay at least 1.53msec
    
    lcd_writebyte(0x04);        // Send 00000100 to data pins, set for 8-bit interface (Entry Mode Set)
    _delay_ms(5);               // Delay at least 1.53ms                           
    
    
    //lcd_writecommand(0x38);     // Function Set: 8-bit interface, 2 lines

   // lcd_writecommand(0x0f);     // Display and cursor on
}

/*
  lcd_moveto - Move the cursor to the row and column given by the arguments.
  Row is 0 or 1, column is 0 - 15.
*/
void lcd_moveto(unsigned char row, unsigned char col)
{
    unsigned char pos;
    pos = row * 0x40 + col;
    lcd_writecommand(0x80 | pos);
}

/*
  lcd_stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void lcd_stringout(char *str)
{
    char ch;
    while ((ch = *str++) != '\0')
	lcd_writedata(ch);
}

/*
  lcd_writecommand - Output a byte to the LCD command register.
*/
void lcd_writecommand(unsigned char x)
{
    PORTB &= ~LCD_RS;       // Clear RS for command write
    lcd_writebyte(x);
    lcd_wait();
}

/*
  lcd_writedata - Output a byte to the LCD data register
*/
void lcd_writedata(unsigned char x)
{
    PORTB |= LCD_RS;	// Set RS for data write
    lcd_writebyte(x);
    lcd_wait();
}

/*
  lcd_writebyte - Output a byte to the LCD
*/
void lcd_writebyte(unsigned char x)
{
    //                         
    
    
    PORTD &= ~LCD_Data_D;
    PORTD |= (x & LCD_Data_D);  // Put 8 bits of data in PORTD
    PORTB &= ~(LCD_RW);// Set R/W=0
    PORTC &= ~(LCD_E); // Set E=0
    PORTC |= LCD_E;             // Set E to 1
    PORTC |= LCD_E;             // Extend E pulse > 230ns
    PORTC &= ~LCD_E;            // Set E to 0
}

/*
  lcd_wait - Wait for the BUSY flag to reset
*/
void lcd_wait()
{
#ifdef USE_BUSY_FLAG
    unsigned char bf;
    
    PORTD &= ~LCD_Data_D;       // Set for no pull ups
    DDRD &= ~LCD_Data_D;   // Set for input

    PORTC &= ~(LCD_E); // Set E=0   // Set E=0, R/W=1, RS=0
    PORTB &= ~LCD_RS;    
    PORTB |= LCD_RW;

    do {
        PORTC |= LCD_E;         // Set E=1
        _delay_us(1);           // Wait for signal to appear
        bf = PIND & LCD_Status; // Read status bit
        PORTC &= ~LCD_E;        // Set E=0
    } while (bf != 0);          // If Busy (PORTD, bit 7 = 1), loop

    DDRD |= LCD_Data_D;     // Set PORTD bits for output
#else
    _delay_ms(2);		// Delay for 2ms
#endif
}
