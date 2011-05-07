/*
 * primary.c
 *
 * Created: 23/04/2011 10:29:03 AM
 *  Author: Coen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define BUFFER_SIZE 64

volatile char buffer[BUFFER_SIZE];
volatile unsigned char insert_pos;
volatile unsigned char bytes_in_buffer;

void setup_usart();
void output_char(char c);
void output_string(char* str);

int main(void){
	// Initialization
	DDRD = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF;
	PORTC = 0x00;
	setup_usart();
	sei(); // Enable global interrupts!
	UDR0 = ' ';
	output_string("Ready: Coen McClelland 42363901\n\r");
	for(;;) {
	}
}

void setup_usart() {
	/*Set baud rate */
	UBRR0H = 0x00;
	UBRR0L = 0x81; // 129 - baud rate calculation
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0);
	
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
			c = buffer[insert_pos - bytes_in_buffer
				+ BUFFER_SIZE];
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
		output_string("s: Entering Audio Sweep Mode\n\r");
		PORTC = 0x10;
	} else if (input == 'v') {
		output_string("v: Entering Voltmeter Mode\n\r");
	} else if (input == 'x') {
		output_string("x: Stopping Audio Sweep\n\r");
		PORTC = 0x00;
	} else {
		output_string("Invalid Command: Entering Splash Mode\n\r");
	}
	UDR0 = ' ';
}