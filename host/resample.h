/*
  resample.h
  Nov 21 2007
  David Rowe

  Converts signal between 8 ksamples/sec ulaw to 16 kHz 8 bit unsigned.
*/

#ifndef __RESAMPLE__
#define __RESAMPLE__

#define NTAP   31   /* number of filter taps */     
#define NTAP_2 16

void upsample(
	      unsigned char us16[], /* output 16 kHz unsigned char samples */
	      short           s8[], /* intermediate 8 kHz short samples    */
	                            /* s8[-NTAP_2..num] must be valid      */
	      unsigned char  mu8[], /* input 8kHz mulaw samples            */
	      int num               /* number of samples in mu8[]          */
);

void downsample(
	      unsigned char  mu8[], /* output 8kHz mulaw samples           */
	      short          s16[], /* intermediate 16 kHz short samples   */
	                            /* s16[-NTAP+1..2*num] must be valid   */
	      unsigned char us16[], /* input 16 kHz unsigned char samples  */
	      int num               /* number of samples in mu8[]          */
);
 
#endif
