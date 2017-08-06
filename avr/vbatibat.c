/*
  vbatibat.c
  David Rowe
  20 Sep 2007

  Closed loop control of on-hook voltage and off hook current.

  PB0/PB1.: illustrate control loop, if both active loop is stable
            around set point.  
  PB2.....: cycles for each iteration of loop
  PB3.....: high when off hook
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "iocompat.h"

#define ON_HOOK  0
#define OFF_HOOK 1

#define IBAT_ON_HOOK  250
#define IBAT_OFF_HOOK 240

#define VBAT_SETPOINT 176 /* -48VDC */
#define IBAT_SETPOINT 205 /* 20mA   */

int main (void)
{
    unsigned short Vbat, Ibat, x;
    int state, next_state;

    /* set up PWM --------------------------------------*/

    TCCR1A = 0xc1;	                /* Timer 1 is 10-bit PWM */
    TCCR1B = 0x9;
    OCR1A = 240;
    DDRD = _BV (OC1);	                /* Enable OC1 as output  */

    /* set up Port B and ADC ---------------------------*/

    DDRB  |= (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3);
    ADCSR  = (1<<ADEN) | 0x6;           /* enable ADC, single shot mode, 
					   clk/64   */ 

    state = ON_HOOK;

    while(1) {
	next_state = state;
		
	PORTB |= 1 << PB2;              /* PB2 off */

	/* sample Vbat and Ibat */

	ADMUX  = (1<<ADLAR);            /* Use AREF, left adjust, ch 0   */
	ADCSR |= (1<<ADSC);             /* start ADC                     */
	while(ADCSR & (1<<ADSC));
	Vbat = ADCH;

	ADMUX  = 1 | (1<<ADLAR);        /* Use AREF, left adjust, ch 1   */
	ADCSR |= (1<<ADSC);             /* start ADC                     */
	while(ADCSR & (1<<ADSC));
	Ibat = ADCH;

	PORTB &= ~(1<<PB2);             /* PB2 on  */

	switch(state) {
	case ON_HOOK:

	    PORTB = 0;

	    /* Regulate for constant voltage -----------------*/

	    if (Vbat < VBAT_SETPOINT) {
		/* beneath set point, Vbat too high,
		   reduce PWM on time, increase PWM value */
		OCR1A = 250;			
		//PORTB |=   1<<PB0;          /* PB0 on  */
		//PORTB &= ~(1<<PB1);         /* PB1 off */
	    }
	    else {
		/* above set point, Vbat too low,
		   increase PWM on time, decrease PWM value */
		OCR1A = 200;			
		//PORTB |=   1<<PB1;          /* PB1 on  */
		//PORTB &= ~(1<<PB0);         /* PB0 off */
	    }

	    /* transition to on-hook when loop current flows */

	    if (Ibat < IBAT_OFF_HOOK) {
		next_state = OFF_HOOK;
	    }
	    break;

	case OFF_HOOK:

	    PORTB |= (1 << PB3);

	    /* Regulate for constant current -----------------*/

	    #define LINEAR
	    #ifdef LINEAR
	    x = Ibat - IBAT_SETPOINT;
	    if (x > 255) x = 255;
	    if (x < 200) x = 200;    
	    OCR1A = x;			
	    #else
	    if (Ibat < IBAT_SETPOINT) {
		/* beneath set point, Ibat too high,
		   decrease PWM on time, increase PWM value */
		OCR1A = 250;			
		//PORTB |=   1<<PB0;          /* PB0 on  */
		//PORTB &= ~(1<<PB1);         /* PB1 off */
	    }
	    else {
		/* above set point, Ibat too low,
		   increase PWM on time, decrease PWM value */
		OCR1A = 250;			
		//PORTB |=   1<<PB1;          /* PB1 on  */
		//PORTB &= ~(1<<PB0);         /* PB0 off */
	    }
	    #endif

	    /* transition to on hook when loop current stops */

	    if (Ibat > IBAT_ON_HOOK) {
		next_state = ON_HOOK;
	    }
	    break;
	    
	}

	state = next_state;
    }
    return (0);
}

