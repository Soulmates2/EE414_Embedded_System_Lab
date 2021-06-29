# Embedded Systems Lab2 Metronome_led Makefile
# Source file: Metronome_led.c, userLEDmmap.c, userLEDmmap.h
# Do everything
all:	m1 m2 m3

# Cross compile
m1:
	arm-linux-gnueabihf-gcc -o pushLEDmmap pushLEDmmap.c
m2:
	arm-linux-gnueabihf-gcc -o userLEDmmap userLEDmmap.c
m3:
	arm-linux-gnueabihf-gcc -o Metronome_led Metronome_led.c

# Cross compile 
cok:
	arm-linux-gnueabihf-gcc -o taylor_ok taylor_ok.c -lm
