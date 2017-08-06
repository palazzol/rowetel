/*
  dcdc.c
  David Rowe
  24 Feb 2011

  Atmel AVR program that implement a telephony power supply a DC-DC
  converter.  The input is 12V, the output 0 to -100V.  

  The circuit used is ../sch/dc-dc-converter.pdf.
*/

#include <inttypes.h>
#include <inttypes.h>
#include <avr/io.h>
#include "../sim/dcdc_control.h"

#define PWM_OUTPUT_BIT     1
#define PWM_CYCLE_BIT      2
#define DCDC_CYCLE_BIT     3

uint8_t inline pwm_complete();

uint8_t inline pwm_complete() {
    return TIFR1 & 0x1;
}

int main (void)
{
    int      pwm_cycles, pwm, ring_cycles;
    int      adc_sample;
    int      Vset;

    Vset = -48;

    /* set up PWM --------------------------------------*/

    /* 16 bit Timer/Counter 1, in 8 bit fast PWM mode */
    /* 64kHz PWM with a 16 MHz clock */

    TCCR1A = 0xc1; 
    TCCR1B = 0x9;
    OCR1A = 128;

    /* set up A/D */

    ADMUX   = (1<<ADLAR);            /* Use AREF, left adjust, ch 0   */
    ADCSRA  = (1<<ADSC) | 0x5;       /* single shot mode, clk/64      */
    ADCSRA |= (1<<ADEN);             /* enable ADC                    */

    /* configure output pins ---------------------------*/

    DDRB  |= (1<<PWM_OUTPUT_BIT); 
    DDRB  |= (1<<PWM_CYCLE_BIT); 
    DDRB  |= (1<<DCDC_CYCLE_BIT); 
 	
    /* init a few variables */

    pwm_cycles = ring_cycles = 0;
    adc_sample= 0;

    while(1) {

	/* wait for current PWM cycle to complete */

	while(!pwm_complete());

	TIFR1 |= 0x1; /* reset PWM-cycle-complete flag */

	/* blip pin */
	
	PORTB |= 1<<PWM_CYCLE_BIT;   
	PORTB &= ~(1<<PWM_CYCLE_BIT);

	pwm_cycles++;
	if (pwm_cycles == 64) {
	    pwm_cycles = 0;
	}
    
	/* sample ADC conversion every 4th cycle (15.625kHz) */

	if (pwm_cycles & 0x2) {

	    adc_sample = ADCH;
		
	    /* kick off new ADC conversion */

	    ADCSRA |= (1<<ADSC);	
	}
             
	/* every 63rd cycle (976.5 Hz) iterate control loop */

	if (pwm_cycles == 0) {

	    /* blip pin to measure control sofwtare execution time */

	    PORTB |= 1<<DCDC_CYCLE_BIT;   
	    control_loop(Vset, adc_sample, &pwm);
	    PORTB &= ~(1<<DCDC_CYCLE_BIT);

	    OCR1A = ((unsigned char)pwm) ^ 0xff;

#ifdef RING
	    /* iterate ring generator */

	    ring_cycles++;
	    if (ring_cycles == 15) {
		Vset = -48-40;
	    }
	    if (ring_cycles >= 30) {
		ring_cycles = 0;
		Vset = -48+40;
	    }
#endif
	}

	
    }

    return (0);
}

