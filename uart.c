#include"uart_function.h"

void uart_init(uint16_t ubrr)
{
	UBRRH = (uint8_t) (ubrr>>8);
	UBRRL = (uint8_t) ubrr;
	UCSRA = (1<<U2X);
	UCSRB = (1<<TXEN)|(1<<RXEN)|(1<<RXCIE);
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}
void uart_transmit(uint8_t data)
{
	while(!(UCSRA&(1<<UDRE)));
	UDR = data;
}
void uart_transmit_string(uint8_t *string)
{
	for(uint8_t i = 0; string[i]!='\0';i++)
		uart_transmit(string[i]);
}
void uart_transmit_ul(uint32_t value, uint8_t *string)
{
	ultoa(value, string, 10);
	uart_transmit_string(string);
}
void uart_transmit_int(int16_t value, uint8_t *string)
{
	itoa(value, string, 10);
	uart_transmit_string(string);
}
void uart_transmit_entry(uint32_t a, int16_t b, int16_t c, uint8_t *string)
{
	uart_transmit_ul(a, string);
	uart_transmit(';');
	uart_transmit_int(b, string);
	uart_transmit(';');
	uart_transmit_int(c, string);
	uart_transmit('\n');
	uart_transmit('\r');
}
