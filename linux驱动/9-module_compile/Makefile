release_ver = $(shell uname -r)

obj-m := hello.o

build:kernel_modules

kernel_modules:
	make -C /lib/modules/${release_ver}/build M=${CURDIR} modules
clean:
	rm -f *.o *.ko *.mod.c *.mod.o *.symvers modul*. *cmd *.order *.swp *.tmp*
	#ls -a|grep -v *.c|grep -v Makefile |xargs rm -rf

