This is being developed with a Sparkfun ATSAMD21 dev breakout. USB input is achieved via an OTG adaptor to the microUSB port.

PS/2 output is handled on pins 2 & 3. Those go into PS/2 data and clock respectively. In my case, I stripped a USB cord for its wires and use a passive PS/2 adaptor to convert the wiring.

I program this with an FTDI buddy, that is connected to ports 0 & 1 for serial. Spares me having to unplug USB all the time.

Everything is run through some resistors from PS/2 & Serial data, just in case. The samd21 is seemingly 5v logic tolerant but YMMV. It is not part of its spec sheet.

Power & ground are drawn from PS/2, to save an external adaptor. PS/2 provides 5V/275mA power, so less than USB but enough. Those go into GND & VUSB.

TODO:
* Clean ps2dev init code loop with delays, currently sometimes requires whacking reboot a couple times
* Handle status lights (num/scroll/capslock)
* Mouse support
