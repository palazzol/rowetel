/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: serialrecplay.c
  AUTHOR......: David Rowe                                           
  DATE CREATED: 14 Feb 2010
                                                                             
  Full duplex record and play of audio from a serial device, runs on an
  x86 Linux PC.  To make this work properly #define BUF_SZ 1800 was reqd
  in Arduino uart2analog.c, as PC would go away and do other things for a
  while.  This means proper echo cancellation is probably not possible,
  limiting the use of this user-mode program to testing.

  Serial code based on ser.c sample from http://www.captain.at

  Compile with:

    gcc serialrecplay.c -o serialrecplay -Wall -g -O2 -lm

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2011 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>  
#include <stdio.h>  
#include <stdlib.h> 
#include <math.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h>  
#include <errno.h>  
#include <termios.h>

#define N        160
#define FS       8000
#define PI       3.141593

int initport(int fd) {
    struct termios options;

    // Set the options for the port...

    cfmakeraw(&options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);

    return 1;
}

int getbaud(int fd) {
    struct termios termAttr;
    int     inputSpeed = -1;
    speed_t baudRate;

    tcgetattr(fd, &termAttr);

    /* Get the input speed */

    baudRate = cfgetispeed(&termAttr);
    switch (baudRate) {
	case B0:      inputSpeed = 0; break;
	case B50:     inputSpeed = 50; break;
	case B110:    inputSpeed = 110; break;
	case B134:    inputSpeed = 134; break;
	case B150:    inputSpeed = 150; break;
	case B200:    inputSpeed = 200; break;
	case B300:    inputSpeed = 300; break;
	case B600:    inputSpeed = 600; break;
	case B1200:   inputSpeed = 1200; break;
	case B1800:   inputSpeed = 1800; break;
	case B2400:   inputSpeed = 2400; break;
	case B4800:   inputSpeed = 4800; break;
	case B9600:   inputSpeed = 9600; break;
	case B19200:  inputSpeed = 19200; break;
	case B38400:  inputSpeed = 38400; break;
	case B57600:  inputSpeed = 38400; break;
	case B115200:  inputSpeed = 38400; break;
	case B230400:  inputSpeed = 230400; break;
    }

    return inputSpeed;
}

void write_serial(int fd, char *data, int len) {
    int n;
    //printf("  writing %d bytes\n", len);
    n = write(fd, data, len);
    if (n < 0) {
	perror("write failed");
	exit(1);
    }
}

/* note this only works reliably with len == 1 */

void read_serial(int fd, char *data, int len) {
    int n;
    //printf("  reading %d bytes  \n", len);

    n = read(fd, data, len);
    if (n < 0) {
	perror("read failed");
	exit(1);
    }
    //printf("  read %d bytes\n", len);
}

int main(int argc, char **argv) {
    int    fd;
    FILE  *frec, *fplay;
    int    i, len, tone, ret;
    unsigned char rx,tx;

    if (argc < 3) {
	printf("usage: serialrecplay Raw8BitRecFile len (samples)"
	       " [Raw8BitPlayFile]\n");
	exit(0);
    }

    frec = fopen(argv[1], "wb");
    if (frec == NULL) {
	printf("error opening raw file: %s\n", argv[1]);
	exit(1);
    }

    len = atoi(argv[2]);

    fplay = NULL;
    if (argc == 4) {
	fplay = fopen(argv[3], "rb");
	if (fplay == NULL) {
	    printf("error opening raw file: %s\n", argv[3]);
	    exit(1);
	}
    }

    /* open and configure serial port */

    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
	perror("open_port: Unable to open /dev/ttyUSB0 - ");
	exit(1);
    } else {
	fcntl(fd, F_SETFL, 0);
    }
	
    initport(fd);

    tone = 0; tx = 0;
    for(i=0; i<len; i++) {
        read_serial(fd, (char *)&rx, 1);
	fwrite(&rx, sizeof(char), 1, frec);

	if (tone)
	    tx = (char)(120.0*cos(i*400.0*2.0*PI/FS) + 127);
       	if ((argc == 4) && (fplay != NULL))
	    ret = fread(&tx, sizeof(char), 1, fplay);
	write_serial(fd, (char *)&tx, 1);
    }

    close(fd);
    fclose(frec);
    if ((argc == 4) && (fplay != NULL)) 
	fclose(fplay);

    return 0;
}
