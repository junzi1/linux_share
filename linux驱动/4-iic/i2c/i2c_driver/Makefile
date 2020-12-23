ifneq ($(KERNELRELEASE),)

obj-m := i2c_driver.o

#obj-m := buttons.o
else

KDIR := /home/junzi/linux-2.6.31

all:
	make -C ${KDIR} M=${PWD} modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -f *.o *.ko *.mod.c *.mod.o *.symvers modul*.

endif
