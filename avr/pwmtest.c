/*
  pwmtest.c
  David Rowe
  31 August 2007

  Outputs a fixed PWM signal.
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

void ioinit (void)                
{
    unsigned char tmp;

    /* 16 bit Timer/Counter 1, in 8 bit fast PWM mode */
    TCCR1A = 0xc1; 
    TCCR1B = 0x9;
    /* Set PWM value */

    tmp = 128;
    OCR1A = tmp ^ 0xff;

    /* Enable OC1A (PORTB Bit 1) as output */
    DDRB |= 1<<PORTB1;
    PRR = 0;
}

int main (void)
{
    ioinit ();
    while(1);
    return (0);
}

