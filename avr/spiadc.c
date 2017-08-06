/*
  spiadc.c
  David Rowe
  16 Nov 2007

  Takes ADC samples and sends them to the SPI port, used for testing the 
  ATmeg163 as a sound input device.  This software runs on the AVR, the
  x86 host runs spirec.c
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "iocompat.h"

#define BUF_SZ 256

int main (void)
{
    unsigned char rx, *pin, *pout, count;
    int      nbuf;
    unsigned char buf[BUF_SZ];
    unsigned short sample, next_sample;
	
    /* set up PWM --------------------------------------*/

    TCCR1A = 0xc1;             /* Timer 1A is 8-bit PWM */
    TCCR1B = 0x9;
    DDRD = _BV (OC1);          /* Enable OC1 as output  */
    OCR1A = 127;

    /* set up ADC --------------------------------------*/

    DDRB  |= (1<<PB0) | (1<<PB1) | (1<<PB2);
    ADMUX  = (1<<ADLAR) | 0x2;      /* Use AREF, left adjust, ch 2   */
    ADCSR  = (1<<ADSC) | 0x5;       /* single shot mode, clk/64      */
    ADCSR |= (1<<ADEN);             /* start ADC                     */

    /* configure output pins ---------------------------*/

    DDRB |= (1<<PB2);

    /* Set up SPI as slave -----------------------------*/

    SPCR = 0x48; 

    /* configure output pins ---------------------------*/

    DDRB  |= (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB6);
	
    /* set up buffer -----------------------------------*/

    nbuf = 0;
    pin = pout = buf;
    count = 0;

    while(1) {
	if (SPSR & 0x80) {
	    rx = SPDR;
	    SPDR = next_sample;

	    /* spi transfer has ocurred */

	    /* blink PB1 */

	    PORTB |= 1 << PB1;          
	    PORTB &= ~(1<<PB1);         

	    if (nbuf == 0) {
		/* we decline this sample, PC will try again */

		SPDR = 0x00;    /* tell PC sample was declined */
	    }
	    else {
		/* OK we accept this sample */
		
		sample = *pout++;
		if (sample == 0)  /* reserve 0 code for flow control */
		    sample = 1;
		next_sample = sample;
		if (pout == (buf + BUF_SZ))
		    pout = buf;
		nbuf--;
	    }
	}

	if (TIFR & 0x4) {
	    /* PWM cycle has finished */
	
	    /* blink PB2 */

	    TIFR &= ~0x2;

	    count++;

	    /* PWM is output at 32kHz, but we only sample
	       ADC value every 16 kHz */

	    if (count & 0x1) {
		if ((ADCSR & (1<<ADSC)) == 0) {
		    sample = ADCH;
		    ADCSR |= (1<<ADSC);
		    
		    PORTB |= 1 << PB2;          
		    PORTB &= ~(1<<PB2);         

		    if (nbuf < BUF_SZ) {
			*pin++ = sample;
			if (pin == (buf + BUF_SZ))
			    pin = buf;
			nbuf++;
			PORTB &= ~(1 << PB0);          
		    } 
		    else {
			/* signal buffer full */
			PORTB |= 1<<PB0;         
		    }
		}
	    }
	}

    }

    return (0);
}

