CC=/usr/bin/avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega328p
OBJ2HEX=/usr/bin/avr-objcopy 

all: blink.hex pwmtest.hex uart2analog.hex impulse_response.hex dcdc.hex

dcdc.obj: ../sim/dcdc_control.o dcdc.o
	$(CC) $(CFLAGS) ../sim/dcdc_control.o dcdc.o -o $@

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o *~
