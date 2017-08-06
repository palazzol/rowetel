/*
  resample.c
  Nov 21 2007
  David Rowe

  Converts signal between 8 ksamples/sec ulaw to 16 kHz 8 bit unsigned.

  UPSAMPLING
  ----------

  To change the sample rate from 8 to 16 kHz we first zero pad
  the input sequence:

    x[n] x[n+1] x[n+2] ..... -> x[n] 0 x[n+1] 0 x[n+2] ........

  n is the index of the 8 kHz signal

  This introduces as image of the orginal signal in the range
  4..8kHz, to remove this image we low pass filter with the
  FIR filter defined by h[]:

    y[k]   = x[n]*h[0] +    0*h[1] + x[n-1]*h[2] + ........
    y[k+1] =    0*h[0] + x[n]*h[1] + x[n-1]*h[2] + ........

  y[k] is the filter output, k is the index of the 16 kHz signal.

  Hmmm, every 2nd input sample is zero, due to the zero padding.
  Also, every 2nd sample in h[] is 0.  This means that we can
  efficiently filter even and odd samples:

  odd.: y[k]   = x[n]*h[0] + x[n-1]*h[2] + .......
  even: y[k+1] = x[n-NTAP/4+1] * h[NTAP/2]

  DOWNSAMPLING
  ------------

  To change the sample rate from 16 kHz to 8 kHz we low pass filter
  the signal to remove any enery between 4 kHz and 8 kHz, then
  decimate (throw away) every second sample.  If we didn't filter the
  4-8 kHz energy would be reflected onto the 0-4kHz region by the
  decimation.

  z[k] = y[k]*h[0] + y[k-1]*h[1] + y[k-2]*h[2] + .............

  However every second sample of h[] is zero, so we get:

  z[k]  = y[k]*h[0] + y[k-2]*h[2] + y[k-4]*h[4] + ............
  z[k] += y[k-NTAP/2+1] * h[NTAP/2-1]

  As we discard every second sample of z[k] we only need to 
  compute every second sample.
*/

/*

  TODO:

  [ ] Convert float h[] to fixed point at build time by host C
      compiler

  [X] rewrite to take advantage of spares oversampled signal and
      sparse coeffs, could be 4x saving in operations?
*/

#include "mulaw.h"
#include "resample.h"

#define SCALE   8   /* 16 bit signed to 8 bit unsigned conversion */

static float h[] = {
-0.003502,  0.000000,  0.005020, -0.000000,
-0.011468,  0.000000,  0.024963, -0.000000,
-0.049111,  0.000000,  0.092577, -0.000000,
-0.187134,  0.000000,  0.622287,  0.997480,
 0.635016,  0.000000, -0.199129, -0.000000,
 0.103181,  0.000000, -0.057809, -0.000000,
 0.031432,  0.000000, -0.015604, -0.000000, 
 0.006936,  0.000000, -0.003502};

static short filter_up(float *h, short *s)
{
    int   i,k;
    short y;

    y = 0;
    for(i=0, k=0; i<NTAP_2; i++,k+=2)
	y += (short)(h[k] * s[-i]);

    return y;
}

void upsample(
	      unsigned char us16[], /* output 16 kHz unsigned char samples */
	      short           s8[], /* intermediate 8 kHz short samples    */
	                            /* s8[-NTAP_2..num] must be valid      */
	      unsigned char  mu8[], /* input 8kHz mulaw samples            */
	      int num               /* number of samples in mu8[]          */
) 
{
    int   n;
    short y;
    
    for(n=0; n<num; n++) {
	s8[n] = mu2lin[mu8[n]];

	/* even sample */

	y = filter_up(h, &s8[n]);
	y = y + (1<(SCALE-1));             /* add half LSB to round         */
	y = (y>>SCALE) + 127;              /* scale and convert to unsigned */
	*us16++ = (unsigned char)y;

	/* odd sample */

	y = (short)(h[NTAP_2-1] * s8[n-NTAP_2/2+1]);
	y = y + (1<(SCALE-1));             /* add half LSB to round         */
	y = (y>>SCALE) + 127;              /* scale and convert to unsigned */
	*us16++ = (unsigned char)y;
    }
}

static short filter_down(float *h, short *s)
{
    int   i,k;
    short y;

    y = 0;
    for(i=0, k=0; i<NTAP; i+=2,k+=2)
	y += (short)(h[k] * s[-i]);

    return y;
}

void downsample(
	      unsigned char  mu8[], /* output 8kHz mulaw samples           */
	      short          s16[], /* intermediate 16 kHz short samples   */
	                            /* s16[-NTAP+1..2*num] must be valid   */
	      unsigned char us16[], /* input 16 kHz unsigned char samples  */
	      int num               /* number of samples in mu8[]          */
) 
{
    int   n;
    short z;
    
    for(n=0; n<num; n++) {
	s16[2*n] = us16[2*n] - 127;
	s16[2*n+1] = us16[2*n+1] - 127;
	z  = filter_down(h, &s16[2*n]);
	z += (short)(h[NTAP_2-1] * s16[2*n-NTAP_2+1]);
	z *= (1<<SCALE);
	mu8[n] = lin2mu[(unsigned short)(z) >> 2];
    }
}
