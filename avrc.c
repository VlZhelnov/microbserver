#define F_CPU 1000000UL
//#define UART_BAUD 9600
//#define UBRR_VALUE F_CPU/UART_BAUD/8-1
#include <avr/io.h>
#include <util/delay.h>
//#include <avr/interrupt.h>
//#include <string.h>
//#include <stdlib.h>

/*
void uart_init(uint16_t ubrr)
{
	UBRRH = (uint8_t) (ubrr>>8);
	UBRRL = (uint8_t) ubrr;
	UCSRA = (1<<U2X);
	UCSRB=(1<<TXEN)|(1<<RXEN);
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}
void uart_transmit(uint8_t data)
{
	while(!(UCSRA&(1<<UDRE)));
	UDR = data;
}
uint8_t uart_receive(void)
{	
	while(!(UCSRA&(1<<RXC)));
	return UDR;
}*/

void pwm_timer_init(void)
{
	PORTB = 0x00;
	DDRB = 0x08;
	ASSR   = 0x00;
	TCCR2  = 0b01101101;
	TCNT2  = 0x00;
	OCR2   = 0x00;
	TIMSK |= 0x00;
}
void adc_init(void)
{
	ADMUX  = 0b01000000;
	ADCSRA = 0b10000100;
}

uint16_t adc_convert(void)
{
	ADCSRA |= (1 << 6);
	while (ADCSRA & (1 << 6));
	return ADC;
}
int main(void)
{
	//uart_init(UBRR_VALUE);
	pwm_timer_init();
	adc_init();
	while(1)
	{
		if (adc_convert() > 650)
			OCR2 = 128;
		else if (adc_convert()>700)
			OCR2 = 190;
		else if (adc_convert()>750)
			OCR2 = 255;
		else
			OCR2 =0x00;
		_delay_ms(100);
	}
}
