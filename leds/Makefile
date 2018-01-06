all:
	bcc -i -Md -ansi -0 -I -I. -V -W -x leds.c -o leds.com
	../Makehex330 leds
	ls -al leds.com leds.hex
