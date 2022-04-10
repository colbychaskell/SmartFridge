#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

void lcd_init(void);
void lcd_moveto(unsigned char, unsigned char);
void lcd_stringout(char *);
void lcd_writecommand(unsigned char);
void lcd_writedata(unsigned char);
void lcd_writebyte(unsigned char);
void lcd_wait(void);

// char str1[] = ">>> at328-4.c <<78901234";
// char str2[] = ">> USC EE459L <<78901234";

#define USE_BUSY_FLAG  1


#define LCD_RS          (1 << PB2)
#define LCD_RW          (1 << PB1)
#define LCD_E           (1 << PD3)
#define LCD_Bits        (LCD_RS|LCD_RW|LCD_E)




#define LCD_Data_C     0x01     // Bits in Port C for LCD data (00000001)
#define LCD_Data_D     0xF7     // Bits in Port D for LCD data (11110111)
#define LCD_Status     (1 << PC0) // Bit in Port D for LCD busy status

