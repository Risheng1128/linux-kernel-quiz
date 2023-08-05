obj-m += \
	bytestream-example.o inttype-example.o record-example.o

KERNELDIR ?= /lib/modules/`uname -r`/build
PWD       := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

check:
	sudo dmesg --clear
	sudo insmod bytestream-example.ko
	sudo rmmod bytestream-example.ko
	sudo insmod inttype-example.ko
	sudo rmmod inttype-example.ko
	sudo insmod record-example.ko
	sudo rmmod record-example.ko
	dmesg > out

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm out
