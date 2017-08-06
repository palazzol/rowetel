/*
  vbatt.c
  David Rowe
  15 Sep 2007

  First attempt at closed loop control of on-hook voltage.  Works!  Can
  produce Vbat = -48V from Vunreg = 6V up, into a 10k load.
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "iocompat.h"

int main (void)
{
	unsigned short sample;
	
	/* set up PWM --------------------------------------*/

	TCCR1A = 0xc1;	                /* Timer 1 is 10-bit PWM */
	TCCR1B = 0x9;
	OCR1A = 240;
	DDRD = _BV (OC1);	        /* Enable OC1 as output  */

	/* set up ADC --------------------------------------*/

	DDRB  |= (1<<PB0) | (1<<PB1) | (1<<PB2);
	ADMUX  = (1<<ADLAR);            /* Use AREF, left adjust, ch 0   */
	ADCSR  = (1<<ADSC) | 0x6;       /* single shot mode, clk/64      */ 
	ADCSR |= (1<<ADEN);             /* start ADC                     */

	while(1) {
		while(ADCSR & (1<<ADSC));
		PORTB &= ~(1<<PB2);         /* PB2 on  */

		sample = ADCH;
		if (sample < 176) {
			/* beneath set point, Vbat too high,
			   reduce PWM on time, increase PWM value */
			OCR1A = 250;			
			PORTB |=   1<<PB0;  /* PB0 on  */
			PORTB &= ~(1<<PB1); /* PB1 off */
		}
		else {
			/* above set point, Vbat too low,
			   increase PWM on time, decrease PWM value */
			OCR1A = 200;			
			PORTB |=   1<<PB1;  /* PB1 on  */
			PORTB &= ~(1<<PB0); /* PB0 off */
		}
		ADCSR |= (1<<ADSC);
		PORTB |= 1 << PB2;          /* PB2 off */
	}
	return (0);
}

