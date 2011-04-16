/*
 * main.c
 *
 * Created: 16/04/2011 3:01:03 PM
 *  Author: Coen
 */ 

#include <avr/io.h>

int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code
		//BLAHBLAHBLAH
		DDRD = 0xFF;
		int count = 0;
		int sintbl[16] = {0x80, 0xB1, 0xDA, 0xF5, 0xFF, 0xF5, 0xDA, 0xB1, 0x80, 0x4F, 0x26, 0x0B, 0x00, 0x0B, 0x26, 0x4F};
		while(1)
		{
			PORTD = sintbl[count];
			count++;
			if (count == 16) {
				count = 0;	
			}					
		 
		} 
    }
}