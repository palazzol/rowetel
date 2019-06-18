$10 ATA README
David Rowe 23 Nov 2007

A low cost FXS port that can be constructed from easy to find analog
parts and a microcontroller.

Status
------

- avr-test/uart2analog.c works on Arduino Duemilanove
- talks to host/serialrecplay.c to play and record files
- sch/hybrid.sch design works OK with uC, bidirectional audio

Task List
---------

[X] Vbatt closed loop regulation
[ ] Vring generation
[ ] DC-DC
    [ ] plot PWM codes, make sure all codes being used
    [X] work out why current draw is so high
        + it's OK, need to compare power not current from 12V rail
    [ ] replace fuse with low value resistor that will smoke up
        + cheaper to replace
    [ ] note on capacitors to stop hard on operation
    [ ] try simpler circuit, garden variety parts
    [ ] measure CPU load of DC-DC at high and low sample rates
    [ ] can we used CFL bulb parts?
[ ] interface to router
    [ ] full duplex speech I/O with echo cancelling
    [ ] hook detection and ring genration GPIOs 
[ ] clean up audio
[ ] custom PCB design
    + clean audio, filtering of AVCC, single point an ground, 
      pads for filter components
[ ] OpenWRT patches for a couple of routers
[ ] Hybrid
    [ ] check C8/C9 capacitor voltage ratings while ringing, might
        need to be 100V

References
----------

Blog posts: 
	Part 1:            http://www.rowetel.com/?p=26
	Part 2:            http://www.rowetel.com/?p=44
	Part 3 - Hybrid:   http://www.rowetel.com/?p=1776
	Part 4 - DC-DC:    http://www.rowetel.com/?p=1807
	Part 5 - Ringing:  http://www.rowetel.com/?p=1987
Gyrator DC Characteristic: http://www.rowetel.com/?p=51

Alternate SLIC:
	Schematic:         http://www.rowetel.com/downloads/SLICN.pdf
	Expired Patent:    http://www.rowetel.com/downloads/slic_4281219.pdf

Google Group (Inactive):   http://groups.google.com/group/low-cost-ata/topics

Directories
-----------

avr.....:  Micro-controller programs
data....:  Data sheets and app notes
dia.....:  figures drawn in dia
host....:  host processor code (x86 or eventually WRT54G)
sim.....:  Octave and C simulations
sch.....:  gEDA gschem schematics

Programming AVR using avrdude on Arduino Duemilanove
----------------------------------------------------

$ sudo avrdude -p m328p -P /dev/ttyUSB0 -c arduino -b 57600 -F -u -U flash:w:blink.hex

Playing Raw files
-----------------

16 bit:

  $ play -r 16000 -s b -f u ../host/test.raw

8 bit unsigned from serialrecplay:

  $ play -r 8000 -u -1 test.raw


