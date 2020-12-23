ifneq ($(KERNELRELEASE),)

obj-m += ov9650_dev.o
obj-m += ov9650_drv.o

else

KDIR := /home/junzi/linux-2.6.31

all:
	make -C ${KDIR} M=${PWD} modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -f *.o *.ko *.mod.c *.mod.o *.symvers modul*.

endif
