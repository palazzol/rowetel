/*
  dcdc_sim.c
  David Rowe 
  23 Feb 2011

  Simulates uC control software driving DC-DC converter.  DC-DC converter
  is simulated using floating point discrete time model.

  To compile:

      gcc dcdc_sim.c dcdc_control.c -o dcdc_sim -Wall -g

  Use dcdc_sim_plot.m to plot the results, and compare to the floating
  point simulation dcdc.m

*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "dcdc_control.h"

#define K       (-2.7/127)
#define FS      1000
#define TAU     0.028         /* experimentally measured, roughly RC */
#define C       (1/(TAU*FS))
#define N       (FS*0.125)

#define KV     -5.58          /* maps voltage to PWM value */
#define GAIN    8

#define VRINGP  40


/* discrete time model of DC-DC converter */

void dcdc_model(unsigned char pwm, float *Vbatt) 
{
    float ret;

    ret = *Vbatt + K * (float)pwm - C * *Vbatt;

    *Vbatt = ret;
}


/* model of ADC */

unsigned char adc_model(float Vbatt) 
{
    float adc = 247.3 + 1.50*Vbatt;
    if (adc < 0.0) adc = 0; 
    if (adc > 255.0) adc = 255; 

    return (unsigned char)adc;
}



int main(void) {
    FILE *f;
    float Vbatt, Vset;
    int   adc,pwm;
    int   i;
    int   ring_period, ring_half_period, ring_counter;

    /* steady state */

    f = fopen("steady.txt", "wt");
    assert(f != NULL);

    Vbatt = 0.0;
    Vset = -48.0;
   
    for(i=0; i<N; i++) {
	adc = adc_model(Vbatt);
	control_loop(Vset, adc, &pwm);
	dcdc_model((unsigned char)pwm, &Vbatt);
	fprintf(f, "%f\n", Vbatt);
    }

    fclose(f);

    /* ringing */

    f = fopen("ring.txt", "wt");
    assert(f != NULL);

    Vbatt = 0.0;
    ring_period = 0.05*FS;
    ring_half_period = 0.025*FS;
    ring_counter = 0;
    Vset = -48.0 + VRINGP;

    for(i=0; i<N; i++) {

	/* square wave AC ring signal */

	ring_counter++;
	if (ring_counter > ring_half_period) {
	    Vset = -48.0 + -VRINGP;
	}
	if (ring_counter > ring_period) {
	    Vset = -48.0 + VRINGP;
	    ring_counter = 0;
	}
	
	adc = adc_model(Vbatt);
	control_loop(Vset, adc, &pwm);
	dcdc_model((unsigned char)pwm, &Vbatt);
	fprintf(f, "%f\n", Vbatt);
    }

    fclose(f);

    return 0;
}
