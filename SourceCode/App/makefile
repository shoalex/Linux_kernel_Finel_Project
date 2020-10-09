CC = arm-linux-gnueabihf-gcc
CFLAGS = -lcurl -std=gnu99 -Wno-declaration-after-statement
DEPS = rc522App.c
OBJ = rc522App.o

all: rc522App

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rc522App: $(OBJ)
	$(CC) -o $@ $< $(CFLAGS) 
	cp rc522App /mnt/BBG_server

clean: 
	rm -rf rc522App $(OBJ) 
