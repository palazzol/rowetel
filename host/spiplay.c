/*
  spiplay.c
  David Rowe 
  16 Nov 2007

  Plays a raw 8 bit/sample binary file out of a bit bashed SPI port.
  Used for testing audio on an AVR microcontroller.  This program
  talks to the AVR test program spipwm.c.
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/time.h>
#include <unistd.h>

#define BYTES    64000
#define PI       3.141593

#define BASEPORT 0x378 /* lp1 */
#define SS       0x40
#define MOSI     0x20
#define MISO     0x40
#define SCLK     0x10
#define RESET    0x80

unsigned char lpt_data;

void inline set(int signal)
{
  lpt_data |= signal;
  outb(lpt_data, BASEPORT);
}

void inline reset(int signal)
{
  lpt_data &= ~signal;
  outb(lpt_data, BASEPORT);
}

unsigned char spi_txrx(unsigned char tx)
{
  int i;
  unsigned char rx;

  rx = 0;
  reset(SS);
  
  for(i=0; i<8; i++) {
    rx <<= 1;
    if (tx & 0x80)
      set(MOSI);
    else
      reset(MOSI);
    tx <<= 1;

    reset(SCLK);
    if (inb(BASEPORT + 1) & MISO)
      rx |= 1; 
    set(SCLK);
  }

  set(SS);

  return rx;
}

int main(int argc, char *argv[])
{
  unsigned char      tx;
  FILE              *f;
  int                i;

  /* Get access to the ports */
  if (ioperm(BASEPORT, 3, 1)) {perror("ioperm"); exit(1);}

  set(RESET);
  usleep(10000);

  #define FILEIO
  #ifdef FILEIO
  if (argc != 2) {
    printf("usage: spiplay Raw8BitFile\n");
    exit(0);
  }

  f = fopen(argv[1], "rb");
  if (f == NULL) {
    printf("error opening raw file: %s\n", argv[1]);
    exit(1);
  }

  while(fread(&tx, sizeof(char), 1, f) == 1) {
    /* wait for AVR to accept sample */
    while(spi_txrx(tx) == 0x00);
  }

  fclose(f);
  #endif

  //#define TEST
  #ifdef TEST
  for(i=0; i<BYTES; i++) {

    tx = (unsigned char)(127.0 + 64.0*sin((float)i*2.0*PI/32));
    tx = ~tx;

    while(spi_txrx(tx) == 0x00);
  }
  #endif

  /* We don't need the ports anymore */
  if (ioperm(BASEPORT, 3, 0)) {perror("ioperm"); exit(1);}

  return 0;
}

/* end of example.c */
