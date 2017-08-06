#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>

void delayms(uint16_t millis) {
  while ( millis ) {
    _delay_ms(1);
    millis--;
  }
}

int main(void) {
  DDRB |= 1<<PORTB5; /* set PB5 to output */
  while(1) {
    PORTB &= ~(1<<PORTB5); /* LED on */
    delayms(100);
    PORTB |= 1<<PORTB5;    /* LED off */
    delayms(900);
  }
  return 0;
}
