obj-m	:= hello_world.o		
KDIR	:= /root/work/achroimx6q/kernel
PWD		:= $(shell pwd)			

default:
	$(MAKE)	-C	$(KDIR)	SUBDIRS=$(PWD)	modules
clean:
	$(MAKE)	-C	$(KDIR)	SUBDIRS=$(PWD)	clean

