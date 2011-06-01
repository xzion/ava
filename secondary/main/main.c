/*
 * main.c
 *
 * Created: 16/04/2011 3:01:03 PM
 *  Author: Coen McClelland
 *	Student Number: 42363901
 *  Group: 23
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void setup_timer0(void);
void setup_timer1(void);

int tblnum = 0;
int tblpos = 0;
int sweep = 0;
int tbl8[8] = {0x80, 0xDA, 0xFF, 0xDA, 0x80, 0x26, 0x00, 0x26};
int tbl16[16] = {0x80, 0xB1, 0xDA, 0xF5, 0xFF, 0xF5, 0xDA, 0xB1, 0x80, 0x4F, 0x26, 0x0B, 0x00, 0x0B, 0x26, 0x4F};
int tbl64[64] = {0x80, 0x8C, 0x98, 0xA4, 0xB0, 0xBB, 0xC6, 0xD0, 0xD9, 0xE2, 0xE9, 0xF0, 0xF5, 0xF9, 0xFC, 0xFE, 0xFF, 0xFE, 0xFC, 0xF9, 0xF5, 0xF0, 0xE9, 0xE2, 0xD9, 0xD0, 0xC6, 0xBB, 0xB0, 0xA4, 0x98, 0x8C, 0x7F, 0x73, 0x67, 0x5B, 0x4F, 0x44, 0x39, 0x2F, 0x26, 0x1D, 0x16, 0xF, 0xA, 0x6, 0x3, 0x1, 0x00, 0x1, 0x3, 0x6, 0xA, 0xF, 0x16, 0x1D, 0x26, 0x2F, 0x39, 0x44, 0x4F, 0x5B, 0x67, 0x73};


int main(void){
	// Initialization
	DDRD = 0xFF;
	PORTD = 0x00;
	DDRC = 0xEF;
	DDRB = 0xFF;
	PORTC = 0x00;
	
	setup_timer0();
	setup_timer1();
	
	for(;;) {
		/*
		PORTC &= (0<<PORTC3);
		while(!(PINC&(1<<PINC4))) {
			PORTB = 0x01;
			// Do nothing, waiting for order to start!
		}
		*/
		//DDRC = 0xFF;
		PORTB = 0x00;
		sei(); // Enable global interrupts!
		while (!sweep){
			//PORTB = 0x02;
			// Do nothing, waiting for sweep to be over
		}
		cli();
		sweep = 0;
	}
}

ISR(TIMER1_COMPA_vect){
	// Timer 1 compare interrupt: change the frequency once a second
	//OCR1A = 0x61A8; // 25000 (16 step, 50hz) Will change
	tblpos++;
	//tblpos = 2; //Testing
	if(tblpos==10){
		PORTC |= (1<<PORTC3);
		tblpos = 0;
		tblnum = 0;
		sweep = 1;
		PORTD = 0x00;
		return;
	}
	switch(tblpos){
		case 1: // Break
			PORTC &= (0<<PORTC3);
			OCR0A = 0xFF;
			break;
		case 2: // 50Hz - 64 samples
			PORTC |= (1<<PORTC3); // Tell Primary chip to start testing with ADC
			OCR0A = 0x61;
			break;
		case 3: // 100Hz - 64 samples
			OCR0A = 0x30;
			PORTC &= (0<<PORTC3);
			break;
		case 4: // 500Hz - 64 samples
			OCR0A = 0x09;
			PORTC |= (1<<PORTC3);
			break;
		case 5: // 1kHz - 16 samples
			OCR0A = 0x13;
			PORTC &= (0<<PORTC3);
			break;
		case 6: // 2kHz - 16 samples
			OCR0A = 0x09;
			PORTC |= (1<<PORTC3);
			break;
		case 7: // 4kHz - 16 samples
			OCR0A = 0x04;
			PORTC &= (0<<PORTC3);
			break;
		case 8: // 8kHz - 8 samples
			OCR0A = 0x04;
			PORTC |= (1<<PORTC3);
			break;
		case 9: // 16kHz - 8 samples
			OCR0A = 0x01;
			PORTC &= (0<<PORTC3);
			break;
	}
	tblnum = 0;	
}

ISR(TIMER0_COMPA_vect){
	// Timer 1 compare interrupt: time to change output
	switch(tblpos){
		// Choosing the frequency to output. Changes on timer0 interrupt.
		case 1: // Break at the start
			PORTD = 0x00;
			break;
		case 2: // 50Hz
			PORTD = tbl64[tblnum];
			tblnum++;
			if (tblnum == 64) {
				tblnum = 0;	
			}
			break;
		case 3: // 100Hz
			PORTD = tbl64[tblnum];
			tblnum++;
			if (tblnum == 64) {
				tblnum = 0;	
			}
			break;
		case 4: // 500Hz
			PORTD = tbl64[tblnum];
			tblnum++;
			if (tblnum == 64) {
				tblnum = 0;	
			}
			break;
		case 5: // 1kHz
			PORTD = tbl16[tblnum];
			tblnum++;
			if (tblnum == 16) {
				tblnum = 0;	
			}
			break;
		case 6: // 2kHz
			PORTD = tbl16[tblnum];
			tblnum++;
			if (tblnum == 16) {
				tblnum = 0;	
			}
			break;
		case 7: // 4kHz
			PORTD = tbl16[tblnum];
			tblnum++;
			if (tblnum == 16) {
				tblnum = 0;	
			}
			break;
		case 8: // 8kHz
			PORTD = tbl8[tblnum];
			tblnum++;
			if (tblnum == 8) {
				tblnum = 0;	
			}
			break;
		case 9: // 16kHz
			PORTD = tbl8[tblnum];
			tblnum++;
			if (tblnum == 8) {
				tblnum = 0;	
			}
			break;
	}
}

void setup_timer1(void){
	// Initializing values
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10); // Set clear on compare mode (CTC) and 1024 prescaler
	TIMSK1 = (1<<OCIE1A); // Enable interrupt on compare register A (OCR1A)
	OCR1A = 0x4FFF;	// 20000 (once a secondish)
}

void setup_timer0(void){
	// Initializing values
	TCCR0A = (1<<WGM01)|(0<<WGM00); // Set clear on compare mode (CTC)
	TCCR0B = (0<<CS02)|(1<<CS01)|(1<<CS00); // Setting clock prescaler to 64
	TIMSK0 = (1<<OCIE0A); // Enable interrupt on compare register A (OCR0A)
	OCR0A = 0xFF; // Default value, 50Hz
}