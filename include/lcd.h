#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

void lcd_init(void);
void lcd_moveto(unsigned char, unsigned char);
void lcd_stringout(char *);
void lcd_writecommand(char);
void lcd_writedata(char);
void lcd_writebyte(char);
void lcd_wait(void);
void lcd_cleardisplay();

// char str1[] = ">>> at328-4.c <<78901234";
// char str2[] = ">> USC EE459L <<78901234";


#define LCD_RS          (1 << PB2)
#define LCD_E           (1 << PB1)
#define LCD_Bits        (LCD_RS|LCD_E)


#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02
#define LCD_Data_D     0xff     // Bits in Port D for LCD data (11111111)
#define LCD_Status     (1 << PD7) // Bit 7 in Port D for LCD busy status

