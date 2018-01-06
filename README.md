# am186es examples

Here a few examples for Am186 systems, that use e86mon as their monitor program.
e86mon features limited DOS int21 support. My examples are using this.

Ready-To-Use-Hexfiles are included in this repo.

* dumpivt/      tool to dump IVT entries, char/string output to serial
* leds/         example on how to use PIO, onboard LEDs
                timer2 in oneshot mode for delay, char/string output to serial
* AMD/          bin-examples from AMD, source is included in e86mon source tree

# Usage

Just send hex-files to am186 e86mon vial serial port. 
GTKTerm or HyperTerminal will let you do this easily.
See e86mon documentation for details.

# toolchain

I am using dev86/bcc on Linux as my C-Compiler and Assembler. It is able to produce plain
.com files for MS-DOS without any library using. In my examples you can see how to use
DOS-Interrupts from C and write a "library".

I am using AMD's makehex utility to convert .com-files to .hex files for e86mon.
See here for DOS-Versions and linux-port:
https://github.com/spacerace/am186-e86mon_utilities

