CROSS_COMPILE=arm-linux-gnueabihf-
ARCH=arm
KERNELDIR=/usr/src/linux-headers-4.14.71-ti-r80
ccflags-y := -std=gnu99 -Wno-declaration-after-statement
PWD := $(shell pwd)
obj-m :=  lcdi2c_0.o

all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) modules
	cp *.ko /mnt/BBG_server

clean:
	rm -f .ko .o .mod.o .mod.c *.symvers