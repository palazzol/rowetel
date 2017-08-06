/*
  mulaw.h
  Nov 21 2007
  David Rowe

  Sets up Look Up Tables (LUTs) for ulaw to linear conversion.
*/

#ifndef __MULAW__
#define __MULAW__

extern short         mu2lin[256];
extern unsigned char lin2mu[16384];

void mulaw_init(void);

#endif
