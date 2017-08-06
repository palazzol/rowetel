/*
  tresample.c
  22 Nov 2007
  David Rowe

  Unit test program for resampling routines.  Evaluate outputs using
  Octave, for example:

    octave:1> load down.txt;plot(down(1:100))

*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mulaw.h"
#include "resample.h"

#define N 160        /* buffer size              */
#define M 5          /* buffers to process       */
#define AMP 30000    /* amplitude of test signal */
#define PI  3.141593

void test_upsample() {
    short          s;
    short          s8[NTAP_2+N];
    unsigned char  mu8[N];
    unsigned char  us16[N*2];
    int            i,j,t;
    FILE          *f;

    mulaw_init();
    for(i=0; i<NTAP_2; i++)
	s8[i] = 0;
    f = fopen("up.txt", "wt");

//#define UP_TEST_MU
#ifdef UP_TEST_MU
    for(i=-32768; i<32768; i++) {
	unsigned char mu;
	unsigned short code;
	code = (unsigned short)(i)>>2;
	mu = lin2mu[code]; 
	s = mu2lin[mu];
	fprintf(f, "%d\n", s);
    }
#endif

//#define UP_DC
#ifdef UP_DC
    for(j=0; j<M; j++) {
	for(i=0; i<N; i++) {
	    s = AMP;
	    mu8[i] = lin2mu[(unsigned short)(s) >> 2]; 
	}
	upsample(us16, &s8[NTAP_2], mu8, N);
	memcpy(s8, &s8[N], sizeof(short)*NTAP_2);
	for(i=0; i<N; i++)
	    fprintf(f, "%d\n", us16[i]);
    }   
#endif

//#define UP_SINE
#ifdef UP_SINE
    t = 0;
    for(j=0; j<M; j++) {
	for(i=0; i<N; i++) {
	    s = AMP*sin(2.0*PI*t++/41);
	    mu8[i] = lin2mu[(unsigned short)(s) >> 2]; 
	}
	upsample(us16, &s8[NTAP_2], mu8, N);
	memcpy(s8, &s8[N], sizeof(short)*NTAP_2);
	for(i=0; i<2*N; i++)
	    fprintf(f, "%d\n", us16[i]);
    }    
#endif

    fclose(f);

}

void test_downsample() {
    short          s;
    short          s16[NTAP+2*N];
    unsigned char  mu8[N];
    unsigned char  us16[N*2];
    int            i,j,t;
    FILE          *f;

    mulaw_init();
    for(i=0; i<NTAP_2; i++)
	s16[i] = 0;
    f = fopen("down.txt", "wt");

//#define DOWN_DC
#ifdef DOWN_DC
    for(j=0; j<M; j++) {
	for(i=0; i<2*N; i++) {
	    us16[i] = 250; 
	}
	downsample(mu8, &s16[NTAP_2], us16, N);
	memcpy(s16, &s16[N], sizeof(short)*NTAP_2);
	for(i=0; i<N; i++)
	    fprintf(f, "%d\n", mu2lin[mu8[i]]);
    }   
#endif

#define DOWN_SINE
#ifdef DOWN_SINE
    t = 0;
    for(j=0; j<M; j++) {
	for(i=0; i<2*N; i++) {
	    /* wanted 500 Hz plus 6 kHz that should be removed */
	    s = 60*sin(2.0*PI*t*0.5/16.0) + 60*sin(2.0*PI*t*6.0/16.0);
	    t++;
	    us16[i] = (unsigned char)(127 + s);
	}
	downsample(mu8, &s16[NTAP_2], us16, N);
	memcpy(s16, &s16[N], sizeof(short)*NTAP_2);
	for(i=0; i<N; i++)
	    fprintf(f, "%d\n", mu2lin[mu8[i]]);
    }    
#endif

    fclose(f);
}

int main() {

    //test_upsample();
    test_downsample();

    return 0;
}
