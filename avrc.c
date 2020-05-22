#define F_CPU 1000000UL
#define UART_BAUD 9600
#define UBRR_VALUE F_CPU/UART_BAUD/8-1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>


uint32_t time_in_seconds = 0;
uint8_t buffer10[11]={'\0'};
uint8_t *buf_pointer;

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
void uart_transmit_clear_string(uint8_t *string)
{
	while(*string != '\0')
	{
		uart_transmit(*string);	
		*string++ = '\0';
	}
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
	OCR1A = 100;//15625;	
}
uint16_t adc_convert(uint8_t channel)
{
	ADMUX  = 0b01000000 | channel;
	ADCSRA = 0b11000100;
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

ISR(TIMER1_COMPA_vect)
{
	time_in_seconds += 1;
	ultoa(time_in_seconds, buffer10, 10);
	uart_transmit_clear_string(buffer10);
	uart_transmit(' ');
	uart_transmit('-');
	uart_transmit(' ');
	utoa(adc_convert(0),buffer10, 10);
	uart_transmit_clear_string(buffer10);
	uart_transmit(' ');
	uart_transmit('-');
	uart_transmit(' ');
	uint8_t x = (uint8_t) (adc_convert(1)>>2);
	OCR2   = x;
	for(uint8_t i = x;i > 0;i--)
		uart_transmit('|');
		
//	utoa(adc_convert(1),buffer10, 10);	
//	uart_transmit_clear_string(buffer10);
	uart_transmit('\n');
	uart_transmit('\r');
}

void pwm_timer2_init(void)
{
	PORTB = 0x01;
	DDRB = 0x08;
	ASSR   = 0x00;
	TCCR2  = 0b01101011;
	TCNT2  = 0x00;
	OCR2   = 0x00;
	TIMSK |= 0x00;
}

int main(void)
{
	uart_init(UBRR_VALUE);
	timer1_init();
	pwm_timer2_init();
	sei();
	while(1)
	{
	}
}	
