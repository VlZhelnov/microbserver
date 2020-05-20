#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/boot.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <avr/pgmspace.h>

void boot_program_page(uint32_t, uint8_t *data) __attribute__((section(".bootloader")));

uint8_t data[64];

int main(void)
{
	PORTD  = 0x00;
	DDRD   = 0xff;
	ADMUX  = 0b01000000;
	ADCSRA = 0b10000100;
	

	for (uint32_t page=0x0100;page < 0x1f00;page += 0x40)
	{
		for (uint8_t i = 0;i<64;i+=2)
		{
			ADCSRA |= (1 << 6);
			while (ADCSRA & (1 << 6));
			data[i] = (uint8_t) (ADC >> 8);
			data[i+1] = (uint8_t) ADC;
			_delay_ms(7500);
		}
		boot_program_page(page, data);
		PORTD += 1;
	}
	while(1);
}


void boot_program_page(uint32_t page, uint8_t *data)
{
	cli();
	boot_page_erase(page);
	boot_spm_busy_wait();
	boot_rww_enable();
	for (uint8_t i=0; i<SPM_PAGESIZE; i+=2)
	{
		uint16_t w = *data++;
		w += (*data++) << 8;
		boot_page_fill(page + i, w);
	}
	boot_page_write(page);
	boot_spm_busy_wait();
	boot_rww_enable();
	sei();
}


