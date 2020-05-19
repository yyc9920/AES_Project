CC = arm-none-linux-gnueabi-gcc
obj-m  := test_dd.o
KDIR    :=  /root/work/achroimx6q/kernel   # 커널 소스 위치
PWD    := $(shell pwd)	# 모듈 소스 위치

all : test_dd   test_app

test_dd : test_dd.c
	$(MAKE)	-C	$(KDIR)	SUBDIRS=$(PWD)	modules

test_app : test_app.c
	$(CC) -o test_app test_app.c

clean:
	rm	-rf		*.ko	*.mod.*		*.order		*.symvers	*.o
	rm	test_app
