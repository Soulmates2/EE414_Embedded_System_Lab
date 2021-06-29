# Embedded Systems Lab4 test_hrtimer.c Makefile
# Source file: Metronome_led.c, userLEDmmap.c, userLEDmmap.h
# Do everything
all:	check hrt metro

# Native compile 
check_pc:
	gcc -o check_pc check.c -lrt
hrt_pc:
	gcc -o a_test_hrtimer_pc test_hrtimer.c -lrt
metro_pc:
	gcc -o metronome_hrt_sim metronome_hrt_pc.c gpio_led_fu.c key_input_fu.c -lrt

# Cross compile
check:
	arm-linux-gnueabihf-gcc -o check check.c -lrt
hrt:
	arm-linux-gnueabihf-gcc -o a_test_hrtimer test_hrtimer.c -lrt
metro:
	arm-linux-gnueabihf-gcc -o metronome_hrt metronome_hrt.c gpio_led_fu.c key_input_fu.c -lrt