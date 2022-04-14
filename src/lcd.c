#include "lcd.h"

/*
  lcd_init - Do various things to force a initialization of the LCD
  display by instructions, and then set up the display parameters and
  turn the display on.
*/
void lcd_init(void)
{
    DDRD |= LCD_Data_D;               // Set PORTD bits 0-8 for output
    DDRB |= LCD_RS;             
    DDRC |= LCD_E;

    _delay_ms(50);                    // Delay at least 40ms

    lcd_writecommand(0b00111100);           // Send 00111100 to data pins, set for 8-bit interface (Function set)
    _delay_ms(50);                    // Delay at least 39usec

    lcd_writecommand(0b00111100);           // Send 00111100 to data pins, set for 8-bit interface (Function set)
    _delay_ms(50);                    // Delay at least 39usec

    lcd_writecommand(0b00001111);           // Display ON 
    _delay_ms(5);                     // Delay

    lcd_writecommand(0b00000001);           // Display Clear
    _delay_ms(5);                     // Delay at least 1.53msec

    lcd_writecommand(0b00000110);           // Entry Mode Set 00000111
    _delay_ms(5);                     // Delay at least 1.53ms    

    //lcd_writecommand(0x02);                    
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
    while ((ch = *str) != '\0')
    {
      lcd_writedata(ch);
      str++;
    }
}



/*
  lcd_writecommand - Output a byte to the LCD command register.
*/
void lcd_writecommand(char x)
{
    PORTB &= ~LCD_RS;       // Clear RS for command write
    lcd_writebyte(x);
    lcd_wait();
}

/*
  lcd_writedata - Output a byte to the LCD data register
*/
void lcd_writedata(char x)
{
    PORTB |= LCD_RS;	// Set RS for data write
    lcd_writebyte(x);
    lcd_wait();
}

/*
  lcd_writebyte - Output a byte to the LCD
*/
void lcd_writebyte(char x)
{
    PORTD = x;
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
    
    _delay_ms(100);
    DDRD |= LCD_Data_D;     // Set PORTD bits for output
#else
    _delay_ms(2);		// Delay for 2ms
#endif

}
