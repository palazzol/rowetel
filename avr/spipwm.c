/*
  spipwm.c
  David Rowe
  15 November 2007

  Takes SPI data and sends it out of the PWM port, used for testing
  the ATmeg163 as a sound output device.  The program runs on the
  AVR, the x86 host runs spiplay.c.
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

    /* set up PWM --------------------------------------*/

    TCCR1A = 0xc1;             /* Timer 1A is 8-bit PWM */
    TCCR1B = 0x9;
    DDRD = _BV (OC1);          /* Enable OC1 as output  */
    OCR1A = 240;

    /* Set up SPI as slave -----------------------------*/

    SPCR = 0x48; 

    /* configure output pins ---------------------------*/

    DDRB  |= (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB6);
	
    /* set up buffer -----------------------------------*/

    nbuf = 0;
    pin = pout = buf;
    count = 0;

    while(1)
    {
	if (SPSR & 0x80) {
	    /* spi transfer has ocurred */

	    /* blink PB1 */

	    PORTB |= 1 << PB1;          
	    PORTB &= ~(1<<PB1);         

	    rx = SPDR;

	    if (nbuf >= (BUF_SZ/2)) {
		/* we decline this sample, PC will try again */

		SPDR = 0x00;    /* tell PC sample was declined */
	    }
	    else {
		/* OK we accept this sample */
		
		SPDR = 0xff;    /* tell PC sample was accepted */
		*pin++ = rx;
		if (pin == (buf + BUF_SZ))
		    pin = buf;
		nbuf++;
	    }

	}

	if (TIFR & 0x4) {
	    /* PWM cycle has finished */
	
	    /* blink PB2 */

	    PORTB |= 1 << PB2;          
	    PORTB &= ~(1<<PB2);         

	    TIFR &= ~0x2;

	    count++;

	    /* PWM is output at 32kHz, but we only update
	       samples at 16 kHz */

	    if (count & 0x1) {
		if (nbuf) {
		    OCR1A = *pout++;	
		    if (pout == (buf + BUF_SZ))
			pout = buf;
		    nbuf--;
		    PORTB &= ~(1<<PB0);         
		}
		else {
		    /* light PB0 to indicate underflow */

		    PORTB |= 1 << PB0;          
		}
	    }
	}

    }

    return (0);
}

