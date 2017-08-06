/*
  spirec.c
  David Rowe 
  16 Nov 2007

  Records 8 bit/samples from a bit bashed SPI port to a file.  Used
  for testing audio input from an AVR microcontroller.  This program
  talks to the AVR program spiadc.c.
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
  unsigned char      rx;
  FILE              *f;
  int                i, len;

  /* Get access to the ports */
  if (ioperm(BASEPORT, 3, 1)) {perror("ioperm"); exit(1);}

  set(RESET);
  usleep(10000);

  if (argc != 3) {
    printf("usage: spirec Raw8BitFile len (samples)\n");
    exit(0);
  }

  f = fopen(argv[1], "wb");
  if (f == NULL) {
    printf("error opening raw file: %s\n", argv[1]);
    exit(1);
  }

  len = atoi(argv[2]);

  for(i=0; i<len; i++) {
    /* wait for AVR to provide sample */
    while((rx = spi_txrx(0x00)) == 0x00);
    fwrite(&rx, sizeof(char), 1, f);
  }

  fclose(f);

  /* We don't need the ports anymore */
  if (ioperm(BASEPORT, 3, 0)) {perror("ioperm"); exit(1);}

  return 0;
}

/* end of example.c */
