## Put here the path to kernel sources! ###
KERNELDIR := /home/costi/Desktop/linux
### Put here the path to toolchain ###
TOOLCHAIN_PREFIX := arm-linux-gnueabihf-
GCC_VERSION := 6.4.1

obj-m := driver.o

PWD := $(shell pwd)

all: empty

empty:
	@echo "Building with kernel sources in $(KERNELDIR)"
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAIN_PREFIX) -C $(KERNELDIR) M=$(PWD) CC=$(TOOLCHAIN_PREFIX)gcc-$(GCC_VERSION) modules
	rm -rf *.o *~ core .depend .*.cmd *.mod *.mod.c .tmp_versions modules.order Module.symvers *.a

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers
