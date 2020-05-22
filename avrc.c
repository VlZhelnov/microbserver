#define F_CPU 1000000UL
#define UART_BAUD 9600
#define UBRR_VALUE F_CPU/UART_BAUD/8-1
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <string.h>
#include <stdlib.h>


uint32_t minuta = 0;

unsigned char str_minuta[11];

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
}


void timer1_init(void)
{
	TCCR1B |= (1<<WGM12) | 3;
	TIMSK |= (1<<OCIE1A);
	OCR1A = 15625;	
}

ISR(TIMER1_COMPA_vect)
{
	minuta += 1;
	unsigned char *p = str_minuta;
	while (*p != '\0') *p++ = '\0'; 
	utoa(minuta, str_minuta,10);
	p = str_minuta;
	while (*p != '\0') uart_transmit(*p++);
	uart_transmit('\n');
	uart_transmit('\r');
}


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
	uart_init(UBRR_VALUE);
	timer1_init();
	sei();
	while(1)
	{
	}
}	
