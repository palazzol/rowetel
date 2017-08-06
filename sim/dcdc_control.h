/*
  dcdc_control.h
  David Rowe 
  24 Feb 2011

  Control loop for DC-DC converter.  Used for C simulation and actual
  microcontroiller real-tme code.

*/

#ifndef __DCDC_CONTROL__
#define __DCDC_CONTROL__

void control_loop(int Vset, int adc, int *pwm);

#endif
