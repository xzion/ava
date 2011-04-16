/*
 * main.c
 *
 * Created: 16/04/2011 3:01:03 PM
 *  Author: Coen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int count = 0;
int sintbl[16] = {0x80, 0xB1, 0xDA, 0xF5, 0xFF, 0xF5, 0xDA, 0xB1, 0x80, 0x4F, 0x26, 0x0B, 0x00, 0x0B, 0x26, 0x4F};

int main(void)
{
    while(1){
		// Initialization
		DDRD = 0xFF;
		TCCR1B = (1<<WGM13)|(1<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10); // Set clear on compare mode (CTC) and no clock prescaler
		TIMSK1 = (1<<OCIE1A); // Enable interrupt on compare register A (OCR1A)
		OCR1A = 0x61A8; // 25000 (16 step, 50hz) Will change
		
		sei(); // Enable global interrupts!
		
		for(;;) {
			// Do nothing - the interrupt handlers take care of it.
		}
    }
}

ISR(TIMER1_COMPA_vect){
	// Timer 1 compare interrupt: time to change output
	PORTD = sintbl[count];
	count++;
	if (count == 16) {
		count = 0;	
	}
}