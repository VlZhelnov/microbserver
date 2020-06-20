#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "uart_function.h"

#define LEN_REQUEST 11
#define COUNT_ENTRIES 5
#define COUNT_SELSIUS 166
#define SELSIUS_OFFSET 40

void timer_init(void);
int16_t midian(int16_t a, int16_t b, int16_t c);
uint8_t offset(int8_t idx, int8_t offset, uint8_t len_arr);
uint16_t adc_convert(uint8_t channel);
uint16_t part_str_to_uint16(uint8_t start, uint8_t end, uint8_t *string);
extern uint16_t bin_search(uint16_t *array, uint16_t len, uint16_t val);

volatile int8_t timer_cursor = 0;
volatile int8_t main_cursor = 0;
volatile int8_t request_idx = 0;
volatile int8_t new_request = 0;

volatile uint32_t time_in_seconds;
volatile uint16_t count_of_record;
volatile uint16_t interval_of_record;
volatile uint16_t interval_ovr;


struct entry{
	uint32_t interval;
	uint16_t temperature;
	uint16_t light;
};

volatile struct entry entries[COUNT_ENTRIES];
uint8_t request[LEN_REQUEST];
uint16_t temp_celsius[COUNT_SELSIUS] = {
	48,  51,  54,  57,  60,  63,  66,  70,  73,  77,
	80,  85,  89,  93,  97,  102, 107, 112, 117, 122,
	127, 133, 139, 144, 150, 156, 163, 169, 176, 183,
	189, 197, 204, 212, 219, 227, 235, 243, 252, 260,
	268, 277, 286, 294, 303, 312, 321, 330, 340, 349,
	358, 368, 377, 387, 397, 406, 416, 425, 435, 445,
	455, 464, 474, 483, 493, 503, 512, 522, 531, 540,
	550, 559, 568, 577, 586, 595, 604, 612, 621, 629,
	638, 646, 654, 662, 670, 678, 685, 693, 700, 708,
	715, 722, 729, 735, 742, 749, 755, 761, 767, 774,
	780, 785, 791, 796, 802, 807, 812, 817, 822, 826,
	831, 836, 840, 844, 849, 853, 857, 861, 865, 868,
	872, 876, 879, 883, 886, 889, 892, 895, 898, 901,
	904, 907, 910, 912, 915, 918, 920, 922, 925, 927,
	929, 931, 933, 935, 937, 940, 941, 943, 945, 947,
	949, 950, 952, 953, 955, 956, 958, 959, 961, 962,
	964, 965, 966, 967, 968, 970
};

ISR(USART_RXC_vect)
{
	uint8_t tmp = UDR; 
	if (((tmp >= '0') && (tmp <= '9')) &&
	    (request[0] == 'G') ||
	    ((tmp == 'G') && (request_idx == 0)))
	{
		request[request_idx++] = tmp;
		uart_transmit(tmp);
	}
	if (request_idx == LEN_REQUEST){
		new_request = 1;
		UCSRB &= ~(1<<RXCIE);
		uart_transmit('\n');
		uart_transmit('\r');
	}
}
ISR(TIMER1_COMPA_vect)
{
	time_in_seconds++;
	interval_ovr++;

	if((interval_ovr == interval_of_record) && (count_of_record > 0)){
		if (timer_cursor >= COUNT_ENTRIES) timer_cursor = 0;
		entries[timer_cursor].interval = time_in_seconds;
		entries[timer_cursor].temperature = adc_convert(0);
		entries[timer_cursor].light = adc_convert(1);
		timer_cursor++;
		count_of_record--;
		interval_ovr = 0;
	}
}
int main(void)
{
	struct entry a,b,c;
	uint8_t conversion_buffer[10];
	uint16_t values[8] = {0};

	uart_init(UBRR_VALUE);
	timer_init();
	sei();

	while(1)
	{
		if (new_request)
		{
			count_of_record = part_str_to_uint16(1,6,request);
			interval_of_record = part_str_to_uint16(6,11,request);
			time_in_seconds = 0;
			interval_ovr = 0;
			new_request = 0;
			request_idx = 0;
			request[0] = 0;
		}
		if (timer_cursor != main_cursor)
		{	
			if (main_cursor >= COUNT_ENTRIES) main_cursor = 0;

			a  = entries[offset(main_cursor,-2,COUNT_ENTRIES)];
			b  = entries[offset(main_cursor,-1,COUNT_ENTRIES)];
			c  = entries[main_cursor];

			values[0] = midian(a.temperature, b.temperature, c.temperature);
			values[2] = midian(a.light, b.light, c.light);
			for (uint8_t i = 0; i < 4; i += 2)
			{
				if (values[i+1] == 0) values[i+1]= values[i];
				values[i] = values[i] * 0.15 + values[i+1] * 0.85;
				values[i+1]= values[i];
			}
			volatile uint16_t index = bin_search(temp_celsius, COUNT_SELSIUS, values[0]);
			uart_transmit_entry(c.interval,(int16_t)index - SELSIUS_OFFSET,
					    values[2]/10, conversion_buffer);
			main_cursor++;
		}
		UCSRB |= (1<<RXCIE);
	}
}

uint16_t part_str_to_uint16(uint8_t start, uint8_t end, uint8_t *string)
{
	uint8_t tmp_arr[end-start];
	uint8_t current_index = 0;
	for (uint8_t i = start; i < end; i++)
		tmp_arr[current_index++] = string[i];
	return atoi(tmp_arr);

}
uint8_t offset(int8_t cursor, int8_t offset, uint8_t len_arr)
{
	return ((cursor+offset) < 0) ? len_arr+cursor+offset : cursor+offset;
}

void timer_init(void)
{
	TCCR1B |= (1<<WGM12) | 3;
	TIMSK |= (1<<OCIE1A);
	OCR1A = 15625;	
}

uint16_t adc_convert(uint8_t channel)
{
	ADMUX  = 0b01000000 | channel;
	ADCSRA = 0b11000100;
	while(ADCSRA & (1 << ADSC));
	return ADC;
}


int16_t midian(int16_t a, int16_t b, int16_t c)
{
	int16_t middle;
	if((a <= b) && (a <= c)){
		middle = (b <= c) ? b : c;
	} else {
		if((b <= a) && (b <= c)){
			middle = (a <= c) ? a : c;
		} else
			middle = (a <= b) ? a : b;
	}
	return middle;
}
