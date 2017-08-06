/*
  adctest.c
  David Rowe
  15 Sep 2007

  Compares the ADC to two thresholds, lights a LED on PB0 if beneath
  threshold, lights a LED on PB1 if above.  Blinks PB2 on every ADC
  conversion.
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "iocompat.h"

int main (void)
{
	unsigned short sample;

	DDRB  |= (1<<PB0) | (1<<PB1) | (1<<PB2);
	ADMUX  = (1<<ADLAR);            /* Use AREF, left adjust, ch 0   */
	ADCSR  = (1<<ADSC) | 0x6;       /* single shot mode, clk/64      */ 
	ADCSR |= (1<<ADEN);             /* start ADC                     */

	while(1) {
		while(ADCSR & (1<<ADSC));
		PORTB &= !(1<<PB2);         /* PB2 on  */

		sample = ADCH;
		if (sample < 0x80) {
			PORTB |=   1<<PB0;  /* PB0 on  */
			PORTB &= ~(1<<PB1); /* PB1 off */
		}
		else {
			PORTB |=   1<<PB1;  /* PB1 on  */
			PORTB &= ~(1<<PB0); /* PB0 off */
		}
		ADCSR |= (1<<ADSC);
		PORTB |= 1 << PB2;          /* PB2 off */
	}
	return (0);
}

