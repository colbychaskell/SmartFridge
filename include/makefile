DEVICE     = atmega328p
CLOCK      = 9830400
PROGRAMMER = -c usbtiny -P usb
OBJECTS    = main.o i2c.o ds1631.o lcd.o pcf8563.o bh1750.o tsl2591.o
SRC		   = src
INCLUDE    = include
FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0xe0:m

# Fuse Low Byte = 0xe0   Fuse High Byte = 0xd9   Fuse Extended Byte = 0xff
# Bit 7: CKDIV8  = 1     Bit 7: RSTDISBL  = 1    Bit 7:
#     6: CKOUT   = 1         6: DWEN      = 1        6:
#     5: SUT1    = 1         5: SPIEN     = 0        5:
#     4: SUT0    = 0         4: WDTON     = 1        4:
#     3: CKSEL3  = 0         3: EESAVE    = 1        3:
#     2: CKSEL2  = 0         2: BOOTSIZ1  = 0        2: BODLEVEL2 = 1
#     1: CKSEL1  = 0         1: BOOTSIZ0  = 0        1: BODLEVEL1 = 1
#     0: CKSEL0  = 0         0: BOOTRST   = 1        0: BODLEVEL0 = 1
# External clock source, start-up time = 14 clks + 65ms
# Don't output clock on PORTB0, don't divide clock by 8,
# Boot reset vector disabled, boot flash size 2048 bytes,
# Preserve EEPROM disabled, watch-dog timer off
# Serial program downloading enabled, debug wire disabled,
# Reset enabled, brown-out detection disabled

# Tune the lines below only if you know what you are doing:

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -Iinclude

# symbolic targets:
all: main.hex

# Add files for compilation below
main.o: main.c
	$(COMPILE) -c $< -o $@

lcd.o: $(SRC)/lcd.c $(INCLUDE)/lcd.h
	$(COMPILE) -c $< -o $@

i2c.o: $(SRC)/i2c.c $(INCLUDE)/i2c.h
	$(COMPILE) -c $< -o $@

ds1631.o: $(SRC)/ds1631.c $(INCLUDE)/ds1631.h
	$(COMPILE) -c $< -o $@
	
pcf8563.o: $(SRC)/pcf8563.c $(INCLUDE)/pcf8563.h
	$(COMPILE) -c $< -o $@

bh1750.o: $(SRC)/bh1750.c $(INCLUDE)/bh1750.h
	$(COMPILE) -c $< -o $@
	
tsl2591.o: $(SRC)/tsl2591.c $(INCLUDE)/tsl2591.h
	$(COMPILE) -c $< -o $@

.c.o: 
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

clean:
	rm -f main.hex main.elf $(OBJECTS)

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
