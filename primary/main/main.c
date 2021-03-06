/*
 * primary.c
 *
 * Created: 23/04/2011 10:29:03 AM
 *  Author: Coen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>


#define BUFFER_SIZE 128

void setup_usart();
void setup_adc();
int get_adc();
void output_string(char* str);
void output_char(char c);


/*
#define LCDOUTPUT PORTB

#define LCDWRITE PORTD6
#define LCDREAD PORTD5
#define LCDENABLE PORTD4
#define LCDCOMMAND PORTD3	
#define LCDRESET PORTD2
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

#define T6963_MODE_SET						0x81


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
*/


volatile char buffer[BUFFER_SIZE];
volatile unsigned char insert_pos;
volatile unsigned char bytes_in_buffer;
volatile int sweeping;


int main(void){
	// Initialization
	
	//DDRD = 0xFF;
	//DDRB = 0xFF;	
	//PORTD |= ((1 << LCDWRITE) | (1 << LCDREAD) | (1 << LCDENABLE) | (1 << LCDCOMMAND) | (1 << LCDRESET) | (1 << LCDFONT));
	//PORTB = 0x00;
	DDRC = 0x10;
	PORTC = 0x00;
	/*
	lcdinit();
	lcdcleartext();
	lcdcleargen();
	lcdclearpic();
	
	lcdcoord(0,0);
	lcdwritestring("group 23 woo");
	*/
	setup_usart();
	setup_adc();
	sei(); // Enable global interrupts!
	output_string("mplitude,86,113,193,130,75,92,117,84");
	UDR0 = 'a';
	while(bytes_in_buffer != 0) {
		// nothing, wait for output to complete
	}
	
	/*
	int sweepres[8];
	sweeping = 0;
	int min;
	int max;
	int adc_result, avg;
	*/
	
	
	while(1) {
		/*		
		while(sweeping) {
			
			PORTC |= (1<<PORTC4);
			output_string("waiting\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			while(!(PINC&(1<<PINC3))) {
				// Do Nothing, waiting to start.
			}
			PORTC &= (0<<PORTC4);
			output_string("done\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			// 50Hz
			output_string("1\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			min = 255;
			max = 0;
			while(PINC&(1<<PINC3)) {
				adc_result = get_adc();
				if (adc_result < min) {
					min = adc_result;
				}
				else if (adc_result > max) {
					max = adc_result;
				}
			}
			avg = (max + min)/2;
			sweepres[0] = avg;
			// 100Hz
			output_string("2\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			min = 255;
			max = 0;
			while(!(PINC&(1<<PINC3))) {
				adc_result = get_adc();
				if (adc_result < min) {
					min = adc_result;
				}
				else if (adc_result > max) {
					max = adc_result;
				}
			}
			avg = (max + min)/2;
			sweepres[1] = avg;
			// 500Hz
			output_string("3\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			min = 255;
			max = 0;
			while(PINC&(1<<PINC3)) {
				adc_result = get_adc();
				if (adc_result < min) {
					min = adc_result;
				}
				else if (adc_result > max) {
					max = adc_result;
				}
			}
			avg = (max + min)/2;
			sweepres[2] = avg;
			// 1kHz
			output_string("4\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			min = 255;
			max = 0;
			while(!(PINC&(1<<PINC3))) {
				adc_result = get_adc();
				if (adc_result < min) {
					min = adc_result;
				}
				else if (adc_result > max) {
					max = adc_result;
				}
			}
			avg = (max + min)/2;
			sweepres[3] = avg;
			// 2kHz
			output_string("5\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			min = 255;
			max = 0;
			while(PINC&(1<<PINC3)) {
				adc_result = get_adc();
				if (adc_result < min) {
					min = adc_result;
				}
				else if (adc_result > max) {
					max = adc_result;
				}
			}
			avg = (max + min)/2;
			sweepres[4] = avg;
			// 4kHz
			output_string("6\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			min = 255;
			max = 0;
			while(!(PINC&(1<<PINC3))) {
				adc_result = get_adc();
				if (adc_result < min) {
					min = adc_result;
				}
				else if (adc_result > max) {
					max = adc_result;
				}
			}
			avg = (max + min)/2;
			sweepres[5] = avg;
			// 8kHz
			output_string("7\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			min = 255;
			max = 0;
			while(PINC&(1<<PINC3)) {
				adc_result = get_adc();
				if (adc_result < min) {
					min = adc_result;
				}
				else if (adc_result > max) {
					max = adc_result;
				}
			}
			avg = (max + min)/2;
			sweepres[6] = avg;
			// 16kHz
			output_string("8\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
			min = 255;
			max = 0;
			while(!(PINC&(1<<PINC3))) {
				adc_result = get_adc();
				if (adc_result < min) {
					min = adc_result;
				}
				else if (adc_result > max) {
					max = adc_result;
				}
			}
			avg = (max + min)/2;
			sweepres[7] = avg;
			
			char *resultstring = "mplitude,86,113,193,130,75,92,117,84";
			//sprintf(resultstring, "mplitude,%d,%d,%d,%d,%d,%d,%d,%d");
			output_string("finito\n");
			UDR0 = '\r';
			while(bytes_in_buffer != 0) {
				// nothing, wait for output to complete
			}
		}
		*/		
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
	//PORTB = 0x01;
	if (input ==  's'){
		output_string("mplitude,86,113,193,130,75,92,117,84");
		UDR0 = 'a';
		//sweeping = 1;
 	} else if (input == 'x') {
		//output_string("x: Stopping Audio Sweep\n");
		//sweeping = 0;
		//UDR0 = ' ';
	} else {
		//output_string("Invalid Command: Entering Splash Mode\n\r");
	}
}

/*

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


lcdwritedata(0 & 0xFF);
lcdwritedata(0 >> 8);
lcdwritecom(T6963_SET_GRAPHIC_HOME_ADDRESS);

lcdwritedata(LCDSPACE);
lcdwritedata(0x00);
lcdwritecom(T6963_SET_GRAPHIC_AREA);

lcdwritedata(0);
lcdwritedata(0 >> 8);
lcdwritecom(T6963_SET_TEXT_HOME_ADDRESS);

lcdwritedata(LCDSPACE);
lcdwritedata(0x00);
lcdwritecom(T6963_SET_TEXT_AREA);

lcdwritedata(2);
lcdwritedata(0x00);
lcdwritecom(T6963_SET_OFFSET_REGISTER);

lcdwritecom(T6963_DISPLAY_MODE  | T6963_GRAPHIC_DISPLAY_ON   | T6963_TEXT_DISPLAY_ON);

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

void lcdwritedata(unsigned char data)
{
while(!(lcdcheck()&0x03));
PORTB = data;

PORTD &= ~((1 << LCDWRITE) | (1 << LCDENABLE) | (1 << LCDCOMMAND));
delay();
PORTD |= ((1 << LCDWRITE) | (1 << LCDENABLE) | (1 << LCDCOMMAND));
}

void lcdwritechar(char charCode)
{
	lcddo(charCode - 32);
}

void lcddo(unsigned char x)
{
lcdwritedata(x);
lcdwritecom(T6963_DATA_WRITE_AND_INCREMENT);
}

void lcdsetpointer(unsigned int address)
{
lcdwritechar(address & 0xFF);
lcdwritechar(address >> 8);
lcdwritecom(T6963_SET_ADDRESS_POINTER);
}

void lcdcleartext(void)
{
int i;
lcdsetpointer(0);

for(i = 0; i < 8; i++)
  {
  lcddo(0);
  }
}

void lcdcleargen(void)
{
unsigned int i;
lcdsetpointer(2 << 11);

for(i = 0; i < 256 * 8; i++)
  {
  lcddo(0);
  }
}

void lcdclearpic(void)
{
int i;
lcdsetpointer(2);
for(i = 0; i < LCDAREA; i++)
  {
  lcddo(0x00);
  }
}

void lcdwritestring(char * string)
{
while(*string)
  {
  lcdwritechar(*string++);
  }
}

void lcdcoord(unsigned char x, unsigned char y)
{
unsigned int address;

address = 0 +  x + (LCDSPACE * y);

lcdsetpointer(address);
}


*/










