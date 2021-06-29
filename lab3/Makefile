# Embedded Systems Lab3 algo_metronome Makefile
# Source file: Metronome_led.c, userLEDmmap.c, userLEDmmap.h
# Do everything
# Native compile 
all:	sk sknb ct tt algo


sk: 
	gcc -o test_single_key test_single_key.c
sknb: 
	gcc -o test_single_key_nb test_single_key_nb.c
ct:
	gcc -o create_thread create_thread.c -lpthread
tt:
	gcc -o terminate_thread terminate_thread.c -lpthread
algo:
	gcc -o algo_metronome_tui algo_metronome_tui.c

# Cross compile
metro:
	arm-linux-gnueabihf-gcc -o metronome_tui_thread metronome_tui_thread.c gpio_led_fu.c key_input_fu.c -pthread
