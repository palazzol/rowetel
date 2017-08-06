/*
  impulse_response.c
  David Rowe
  20 Feb 2011

  Atmel AVR program that samples the impulse response of a system
  connected between the PWM port and ADC.  Fires off impulses every
  500ms, samples response, sand sends it a host via RS232.

  The A/D is sampled at 7.8125l kHz, the PWM frequency is 62.5 kHz
  (16MHz clock).

  Note: Impulse generation ocde works well, haven't tested sampling
  side yet, as response was simple enough (1st rdoer exponential) to
  be analysed on oscilliscope.
*/

#include <inttypes.h>
#include <inttypes.h>
#include <avr/io.h>

#define IMPULSE_PERIOD 31200  /* roughly 500ms with a PWM rate of 62.5 kHz */
                              /* multiple of 4 to suit ADC sampling        */
#define PWM_OUTPUT_BIT     1
#define PWM_CYCLE_BIT      2
#define RS232_TX_BIT       4

uint8_t inline pwm_complete();

uint8_t inline pwm_complete() {
    return TIFR1 & 0x1;
}

int main (void)
{
    int      pwm_cycles, adc_cycles;
    int      adc_sample1, adc_sample2;
	
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

    /* set up RS232 UART -------------------------------*/

    // set baud rate

    UBRR0H = 0;
    UBRR0L = 16;             // 115,200 baud with 16 MHz clock +2% error

    UCSR0A = (1<< U2X0);     // double speed mode

    // Enable transmitter

    UCSR0B = (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit

    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

    /* configure output pins ---------------------------*/

    DDRB  |= (1<<PWM_OUTPUT_BIT); 
    DDRB  |= (1<<PWM_CYCLE_BIT); 
    DDRB  |= (1<<RS232_TX_BIT); 
 	
    /* init a few variables */

    pwm_cycles = adc_cycles = 0;
    adc_sample1 = adc_sample2 = 0;

    while(1) {

	/* wait for current PWM cycle to complete */

	while(!pwm_complete());

	TIFR1 |= 0x1; /* reset PWM-cycle-complete flag */

	/* blip pin */
	
	PORTB |= 1<<PWM_CYCLE_BIT;   
	PORTB &= ~(1<<PWM_CYCLE_BIT);

	/* send impulse every IMPULSE_PERIOD output samples */

	if (pwm_cycles == 0) {
	    /* impulse */
	    OCR1A = 0;
	}

	if (pwm_cycles == 2) {
	    OCR1A = 255;
	}

	pwm_cycles++;
	if (pwm_cycles == (IMPULSE_PERIOD-1)) {
	    pwm_cycles = 0;
	}
#ifdef TMP
    
	/* sample ADC conversion every 4th cycle (15.625kHz) */

	if (pwm_cycles & 0x2) {

	    if (adc_cycles == 0)
	        adc_sample1 = ADCH;
	    else
		adc_sample2 = ADCH;

	    /* modulo 2 counter */

	    adc_cycles++;
	    adc_cycles &= 1;
		
	    /* kick off new ADC conversion */

	    ADCSRA |= (1<<ADSC);	
	}
                	    
	/* every 8th cycle (7.8125l kHz) average two ADC samples and 
	   send to host router */

	if (pwm_cycles == 0) {
	    UDR0 = (char)((adc_sample1+adc_sample2)>>1);

	    /* blip pin */

	    PORTB |= 1<<RS232_TX_BIT;   
	    PORTB &= ~(1<<RS232_TX_BIT);
	}
#endif
    }

    return (0);
}

