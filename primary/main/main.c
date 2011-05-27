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

volatile char buffer[BUFFER_SIZE];
volatile unsigned char insert_pos;
volatile unsigned char bytes_in_buffer;
int sweeping = 0;

void setup_usart();
void setup_adc();
int get_adc();
void output_char(char c);
void output_string(char* str);

int main(void){
	// Initialization
	DDRD = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF;
	PORTC = 0x00;
	PORTB = 0x00;
	setup_usart();
	setup_adc();
	sei(); // Enable global interrupts!
	output_string("Ready: Coen McClelland 42363901\n");
	UDR0 = '\r';
	
	int sweepres[8];
	int currstat;
	
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