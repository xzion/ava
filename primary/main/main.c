/*
 * primary.c
 *
 * Created: 23/04/2011 10:29:03 AM
 *  Author: Coen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define BUFFER_SIZE 128
#define F_CPU 8000000

#define LCDOUTPUT PORTB

#define LCDWRITE PORTD2
#define LCDREAD PORTD3
#define LCDENABLE PORTD4
#define LCDCOMMAND PORTD5
#define LCDRESET PORTD6
#define LCDFONT PORTD7
#define LCDROWS 64
#define LCDCOLS 128
#define LCDSPACE (LCDCOLS / LCDROWS)
#define LCDAREA (LCDSPACE * LCDROWS)

#define T6963_SET_CURSOR_POINTER			0x21
#define T6963_SET_OFFSET_REGISTER			0x22
#define T6963_SET_ADDRESS_POINTER			0x24

#define T6963_SET_TEXT_HOME_ADDRESS			0x40
#define T6963_SET_TEXT_AREA					0x41
#define T6963_SET_GRAPHIC_HOME_ADDRESS		0x42
#define T6963_SET_GRAPHIC_AREA				0x43

#define T6963_MODE_SET						0x80


#define T6963_DISPLAY_MODE					0x90
	#define T6963_CURSOR_BLINK_ON			0x01
	#define T6963_CURSOR_DISPLAY_ON			0x02
	#define T6963_TEXT_DISPLAY_ON			0x04
	#define T6963_GRAPHIC_DISPLAY_ON		0x08				


#define T6963_CURSOR_PATTERN_SELECT			0xA0
	#define T6963_CURSOR_1_LINE				0x00
	#define T6963_CURSOR_2_LINE				0x01
	#define T6963_CURSOR_3_LINE				0x02
	#define T6963_CURSOR_4_LINE				0x03
	#define T6963_CURSOR_5_LINE				0x04
	#define T6963_CURSOR_6_LINE				0x05
	#define T6963_CURSOR_7_LINE				0x06
	#define T6963_CURSOR_8_LINE				0x07

#define T6963_SET_DATA_AUTO_WRITE			0xB0
#define T6963_SET_DATA_AUTO_READ			0xB1
#define T6963_AUTO_RESET					0xB2

#define T6963_DATA_WRITE_AND_INCREMENT		0xC0
#define T6963_DATA_READ_AND_INCREMENT		0xC1
#define T6963_DATA_WRITE_AND_DECREMENT		0xC2
#define T6963_DATA_READ_AND_DECREMENT		0xC3
#define T6963_DATA_WRITE_AND_NONVARIALBE	0xC4
#define T6963_DATA_READ_AND_NONVARIABLE		0xC5

#define T6963_SCREEN_PEEK					0xE0
#define T6963_SCREEN_COPY					0xE8



volatile char buffer[BUFFER_SIZE];
volatile unsigned char insert_pos;
volatile unsigned char bytes_in_buffer;
int sweeping = 0;

void setup_usart();
void setup_adc();
int get_adc();
void output_char(char c);
void output_string(char* str);
void lcdwrite(unsigned char data);
void lcdwritecom(unsigned char command);
unsigned char lcdcheck(void);
void delay(void);
void lcdinit(void);

int main(void){
	// Initialization
	
	DDRD = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF;
	PORTD |= ((1 << LCDWRITE) | (1 << LCDREAD) | (1 << LCDENABLE) | (1 << LCDCOMMAND) | (1 << LCDRESET) | (1 << LCDFONT));
	PORTC = 0x00;
	PORTB = 0x00;
	setup_usart();
	setup_adc();
	sei(); // Enable global interrupts!
	output_string("Ready: Coen McClelland 42363901\n");
	UDR0 = '\r';
	
	int sweepres[8];
	//int currstat;
	
	for(;;) {
		
		while(sweeping) {
			/*
			output_string("sweeping\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			*/
			/*DDRC = 0xFF;
			PORTC = 0x10;
			DDRC = 0x00;
			while(!(PINC&(1<<PINC5))) {
				// Do Nothing, waiting to start.
			}*/
			int i;
			for (i = 0; i < 1; i++) {
				//DDRC = 0x00;
				//currstat = PINC;
				/*
				output_string("forloop\n");
				UDR0 = '\r';
				while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
				}
				*/
				int min = 255;
				int max = 0;
				int adc_result, avg;
				char gotout[10];
				
				//while (PINC == currstat) {
				while (sweeping) {
					adc_result = get_adc();
					if (adc_result < min) {
						min = adc_result;
					}
					else if (adc_result > max) {
						max = adc_result;
					}
					
					//sprintf(gotout, "got %d\n", adc_result);
					PORTB = max;
					//output_string(gotout);
					//UDR0 = '\r';
					//while(bytes_in_buffer != 0) {
						// nothing, wait for output to complete
					//}
				}
				sprintf(gotout, "min %d\n", min);
				output_string(gotout);
				UDR0 = '\r';
				while(bytes_in_buffer != 0) {
					// nothing, wait for output to complete
				}
				sprintf(gotout, "max %d\n", max);
				output_string(gotout);
				UDR0 = '\r';
				while(bytes_in_buffer != 0) {
					// nothing, wait for output to complete
				}
				avg = (min + max)/2;
				sweepres[i] = avg;
				sprintf(gotout, "avg %d\n", avg);
				output_string(gotout);
				UDR0 = '\r';
				while(bytes_in_buffer != 0) {
					// nothing, wait for output to complete
				}
			}
		}
		DDRC = 0xFF;
		PORTC = 0x00;
		// DO LCD graph for 10 seconds
	}
}

void setup_usart() {
	/*Set baud rate */
	UBRR0H = 0x00;
	UBRR0L = 0x33; // 51 - baud rate calculation
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0);
	
}

void setup_adc() {
	// Set reference voltage (VCC), location ADC0
	ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR);
	// Enable ADC and interrupt. Clock divider set to 2.
	ADCSRA = (1<<ADEN);
	
}

int get_adc() {
	// Stuff
	ADCSRA |= (1<<ADSC);
	while(!(ADCSRA&(1<<ADIF))){
		// Do nothing, wait for conversion to complete
	}
	int res = ADCH;
	return res;
}

void output_string(char* str) {
	unsigned char i;	/* index into the string */
	for(i=0; str[i] != 0; i++) {
		output_char(str[i]);
	}
}

void output_char(char c) {
	// Add to output buffer
	if(bytes_in_buffer < BUFFER_SIZE) {
		/* We have room to add this byte */
		buffer[insert_pos++] = c;
		bytes_in_buffer++;
		if(insert_pos == BUFFER_SIZE) {
			/* Wrap around buffer pointer if necessary */
			insert_pos = 0;
		}
	} 
	/* else, we have no room to add the byte - just discard it */
}

ISR(USART_TX_vect) {
	/* Check if we have data in our buffer */
	if(bytes_in_buffer > 0) {
		char c;
		if(insert_pos - bytes_in_buffer < 0) {
			/* Need to wrap around */
			c = buffer[insert_pos - bytes_in_buffer	+ BUFFER_SIZE];
		} else {
			c = buffer[insert_pos - bytes_in_buffer];
		}
		/* Decrement our count of the number of bytes in the 
		** buffer 
		*/
		bytes_in_buffer--;
		
		// Output the character via the UART
		UDR0 = c;
	}
	// else nothing to transmit
}

ISR(USART_RX_vect) {
	char input;
	input = UDR0;
	PORTB = 0x01;
	if (input ==  's'){
		output_string("mplitude,5,12,36,2,4,9,47,254");
		sweeping = 1;
		UDR0 = 'a';
	} else if (input == 'v') {
		output_string("v: Entering Voltmeter Mode\n");
		UDR0 = ' ';
	} else if (input == 'x') {
		output_string("x: Stopping Audio Sweep\n");
		sweeping = 0;
		UDR0 = ' ';
	} else {
		//output_string("Invalid Command: Entering Splash Mode\n\r");
	}
}

void delay(void)
{
volatile unsigned char i;
for(i = 0; i < (F_CPU/1000000); i++)
  {
  asm("nop");
  }
}

void lcdinit(void)
{

PORTB &= ~(1 << LCDRESET);
_delay_ms(1);
PORTB |= (1 << LCDRESET);

PORTB &= ~(1 << LCDFONT);


lcdwrite(0 & 0xFF);
lcdwrite(0 >> 8);
lcdwritecom(T6963_SET_GRAPHIC_HOME_ADDRESS);

lcdwrite(LCDSPACE);
lcdwrite(0x00);
lcdwritecom(T6963_SET_GRAPHIC_AREA);

lcdwrite(0);
lcdwrite(0 >> 8);
lcdwritecom(T6963_SET_TEXT_HOME_ADDRESS);

lcdwrite(LCDSPACE);
lcdwrite(0x00);
lcdwritecom(T6963_SET_TEXT_AREA);

lcdwrite(2);
lcdwrite(0x00);
lcdwritecom(T6963_SET_OFFSET_REGISTER);

lcdwritecom(T6963_DISPLAY_MODE  | T6963_GRAPHIC_DISPLAY_ON   | T6963_TEXT_DISPLAY_ON /*| T6963_CURSOR_DISPLAY_ON*/);

lcdwritecom(T6963_MODE_SET | 0);

}

unsigned char lcdcheck(void)
{
int tmp;
DDRB = 0x00;

PORTD &= ~((1 << LCDREAD) | (1 << LCDENABLE));
delay();
tmp = PINB;
DDRB = 0xFF;
PORTD |= ((1 << LCDREAD) | (1 << LCDENABLE));
return tmp;
}

void lcdwritecom(unsigned char command)
{
while(!(lcdcheck()&0x03));
PORTB = command;

PORTD &= ~((1 << LCDWRITE) | (1 << LCDENABLE));
delay();
PORTD |= ((1 << LCDWRITE) | (1 << LCDENABLE));
}

void lcdwrite(unsigned char data)
{
while(!(lcdcheck()&0x03));
PORTB = data;

PORTD &= ~((1 << LCDWRITE) | (1 << LCDENABLE) | (1 << LCDCOMMAND));
delay();
PORTD |= ((1 << LCDWRITE) | (1 << LCDENABLE) | (1 << LCDCOMMAND));
}




