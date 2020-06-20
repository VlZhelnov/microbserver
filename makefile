avrc.elf: avrc.c
	avr-gcc -Os -mmcu=atmega8 avrc.c uart.c bin_search.S -o avrc.elf
avrc.hex: avrc.elf
	avr-objcopy -j .text -j .data -O ihex avrc.elf avrc.hex	
load: avrc.hex
	avrdude  -p m8 -c usbasp -U flash:w:avrc.hex
clear:
	rm -rf *.elf *.hex
