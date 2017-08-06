/*
  uart2analog.c
  David Rowe
  10 Feb 2011

  AVR program that converts RS232 UART samples to analog samples and
  implements a FIFO buffer between the RS232 Rx and PWM (D/A) analog
  port.

  The A/D is sampled at 16kHz.  Two samples are averaged and sent to
  the router UART via RS232 at a 8kHz sample rate.

  RS232 samples are received from the router UART, buffered, then
  output as PWM. The PWM sample rate is 64kHz, we just update the
  output every 8kHz.
*/

#include <inttypes.h>
#include <inttypes.h>
#include <avr/io.h>

#define BUF_SZ 1800

#ifdef DIGITAL_MW
/* used for testing - creates a sinusoidal tone */
static char digital_milliwatt[] = {0x1e,0x0b,0x0b,0x1e,0x9e,0x8b,0x8b,0x9e} ;
#endif

uint8_t inline pwm_complete();

uint8_t inline pwm_complete() {
    return TIFR1 & 0x1;
}

/*
  TODO:

  [ ] upgrade to 10 bit PWM and ADC samples, add companding
*/

/* Port B bits used for PWM output and debug blinkies */

#define PWM_OUTPUT_BIT     1
#define PWM_NEW_SAMPLE_BIT 0
#define PWM_CYCLE_BIT      2
#define RS232_RX_BIT       3
#define RS232_TX_BIT       4
#define FIFO_OVERFLOW_BIT  5

int main (void)
{
    unsigned char *pin, *pout, count;
    int      nbuf, pwm_cycles, adc_cycles;
    unsigned char buf[BUF_SZ];
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
    UBRR0L = 16;            // 115,200 baud with 16 MHz clock +2% error

    UCSR0A = (1<< U2X0);     // double speed mode

    // Enable transmitter an receiver

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit

    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

    /* configure output pins ---------------------------*/

    DDRB  |= (1<<PWM_OUTPUT_BIT); 
    DDRB  |= (1<<RS232_RX_BIT); 
    DDRB  |= (1<<PWM_CYCLE_BIT); 
    DDRB  |= (1<<PWM_NEW_SAMPLE_BIT);
    DDRB  |= (1<<RS232_TX_BIT); 
    DDRB  |= (1<<FIFO_OVERFLOW_BIT); 
	
    /* set up FIFO buffer ------------------------------*/

    nbuf = 0;
    pin = pout = buf;
    count = 0;

    /* init a few variables */

    pwm_cycles = adc_cycles = 0;
    adc_sample1 = adc_sample2 = 0;

    while(1) {

	/* wait for current PWM cycle to complete */

	while(!pwm_complete()) {
	    /* receive bytes from host via RS232 UART */

	    if (UCSR0A & (1<<RXC0)) {
		if (nbuf <= BUF_SZ) {
		    *pin++ = UDR0;
		    if (pin == (buf + BUF_SZ))
			pin = buf;
		    nbuf++;
		}
		else {
		    /* blip fifo overflow pin */

		    PORTB |= 1<<FIFO_OVERFLOW_BIT;   
		    PORTB &= ~(1<<FIFO_OVERFLOW_BIT);
		}

		/* blip pin */

		PORTB |= 1<<RS232_RX_BIT;   
		PORTB &= ~(1<<RS232_RX_BIT);
	    }
	}

	TIFR1 |= 0x1; /* reset PWM-cycle-complete flag */

	/* blip pin */
	
	PORTB |= 1<<PWM_CYCLE_BIT;   
	PORTB &= ~(1<<PWM_CYCLE_BIT);

	/* update PWM output sample every 7th cycle */

	if (pwm_cycles == 7) {

	    if (nbuf) {
		OCR1A = *pout++;
		nbuf--;
	    }
	    else
		OCR1A = 128; /* send 0 if FIFO empty */

	    if (pout == (buf + BUF_SZ))
		pout = buf;
		
            /* blip pin */

            PORTB |= 1 << PWM_NEW_SAMPLE_BIT;
            PORTB &= ~(1<<PWM_NEW_SAMPLE_BIT);
	}
	    
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
                	    
	/* every 8th cycle (7.8125lHz) average two ADC samples and 
	   send to host router */

	if (pwm_cycles == 0) {
	    UDR0 = (char)((adc_sample1+adc_sample2)>>1);

	    /* blip pin */

	    PORTB |= 1<<RS232_TX_BIT;   
	    PORTB &= ~(1<<RS232_TX_BIT);
	}

	/* modulo 8 counter */

	pwm_cycles++;
	pwm_cycles &= 7;
    }

    return (0);
}

