# Embedded Systems Lab5 metro_client.c metro_server.c Makefile
# Source file: Metronome_led.c, userLEDmmap.c, userLEDmmap.h
# Do everything
all:	metro_pc metro

# Native compile 
stream_pc:
	gcc -o server server_ori.c -lrt
	gcc -o client client_ori.c -lrt

datagram_pc:
	gcc -o listener listener.c -lrt
	gcc -o talker talker.c -lrt

BD_pc:
	gcc -o serverBD serverBD.c -lrt -pthread
	gcc -o clientBD clientBD.c -lrt -pthread

BD_sk_pc:
	gcc -o serverBD_sk serverBD_sk.c -lrt -pthread
	gcc -o clientBD_sk clientBD_sk.c -lrt -pthread

metro_pc:
	gcc -o metro_server_sim metro_server.c gpio_led_fu_sim.c server.c -lrt
	gcc -o metro_client_sim metro_client.c key_input_fu.c client.c -lrt -pthread

# Cross compile
stream:
	arm-linux-gnueabihf-gcc -o server server_ori.c -lrt
	gcc -o client client_ori.c -lrt

datagram:
	arm-linux-gnueabihf-gcc -o listener listener.c -lrt
	gcc -o talker talker.c -lrt

BD:
	arm-linux-gnueabihf-gcc -o serverBD serverBD.c -lrt -pthread
	gcc -o clientBD clientBD.c -lrt -pthread

BD_sk:
	arm-linux-gnueabihf-gcc -o serverBD_sk serverBD_sk.c -lrt -pthread
	gcc -o clientBD_sk clientBD_sk.c -lrt -pthread

metro:
	arm-linux-gnueabihf-gcc -o metro_server metro_server.c gpio_led_fu.c server.c -lrt
	gcc -o metro_client metro_client.c key_input_fu.c client.c -lrt -pthread
