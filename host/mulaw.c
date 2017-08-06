/*
  mulaw.c
  Nov 21 2007
  David Rowe

  Sets up Look Up Tables (LUTs) for ulaw to linear conversion.

  TODO:
  [ ] rewrite so that LUTs are generated at build time by host
      C compiler under Makefile control.
*/

#include "mulaw.h"

/* mulaw conversion code from zaptel.c */

#define ZEROTRAP    /* turn on the trap as per the MIL-STD */
#define BIAS 0x84   /* define the add-in bias for 16 bit samples */
#define CLIP 32635

short         mu2lin[256];
unsigned char lin2mu[16384];

static unsigned char lineartomulaw(short sample)
{
  static int exp_lut[256] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
                             4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                             5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                             5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};
  int sign, exponent, mantissa;
  unsigned char ulawbyte;

  /* Get the sample into sign-magnitude. */
  sign = (sample >> 8) & 0x80;                  /* set aside the sign */
  if (sign != 0) sample = -sample;              /* get magnitude      */
  if (sample > CLIP) sample = CLIP;             /* clip the magnitude */

  /* Convert from 16 bit linear to ulaw. */
  sample = sample + BIAS;
  exponent = exp_lut[(sample >> 7) & 0xFF];
  mantissa = (sample >> (exponent + 3)) & 0x0F;
  ulawbyte = ~(sign | (exponent << 4) | mantissa);
  if (ulawbyte == 0) ulawbyte = 0x02;           /* optional CCITT trap */
  if (ulawbyte == 0xff) ulawbyte = 0x7f;        /* never return 0xff   */
  return(ulawbyte);
}

void mulaw_init(void)
{
    int i;

    /* Set up ulaw conversion table */

    for(i=0; i<256; i++)
    {
	short mu,e,f,y;
	static short etab[]={0,132,396,924,1980,4092,8316,16764};

	mu = 255-i;
	e = (mu & 0x70)/16;
	f = mu & 0x0f;
	y = f * (1 << (e + 3));
	y += etab[e];
	if (mu & 0x80) y = -y;
	mu2lin[i] = y;
    }

    /* set up the reverse conversion table */
 
    for(i = -32768; i < 32768; i += 4)
    {
	    lin2mu[((unsigned short)(short)i) >> 2] = lineartomulaw(i);
    }
}

