/*
  dcdc_control.c
  David Rowe 
  24 Feb 2011

  Control loop for DC-DC converter.  Used for C simulation and actual
  microcontroiller real-tme code.

*/

#include <stdio.h>
#include "dcdc_control.h"

/* discrete time control system */

void control_loop(int Vset, int adc, int *pwm)
{
    int          Vdiff;
    int          Vbatt_from_adc;
    int          tmp;
    unsigned int mult;
    
    /*
      Fixed point scaling:

        Vbatt_from_adc = (adc - 247.35)/1.50
                       = adc/1.5 - 164.9

      We can't do divides in real time, so we need to get that nasty
      1.50 of the bottom line, converting any bottom line divides to a
      power of two.  Lets try:

                      = (256/1.5)*adc >> 8 - 164.9

      which is approximately:

                     = ((171*adc)>>8) - 165

    */

    mult = 171 * (unsigned)adc;
    Vbatt_from_adc = (int)( (mult + (1<<7)) >> 8) - 165;
    Vdiff = Vset - Vbatt_from_adc;
 
    /*
      Fixed point scaling:

      pwm = Vdiff*KV*GAIN;
          = Vdiff*(-5.58)*8
	  = Vdiff*(-44.6)
   
      which is approximately:
	
          = -Vdiff * 32
          = -Vdiff << 5
    */

    tmp = -Vdiff << 5;

    /* limit to range of PWM values.  The DC-DC converter doesn't work
       well for "on times" greater than 191, so we limit it there. */

    if (tmp < 0)
	tmp = 0;
    if (tmp > 191)
	tmp = 191;

    *pwm = tmp;
}

