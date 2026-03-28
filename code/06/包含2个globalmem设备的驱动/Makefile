# ------------------------------------------------------------------------------
#
# Makefile for the LDD-LinuxDeviceDrivers.
#
# Author: gatieme
# Create: 2016-07-29 15:50:46
# Last modified: 2016-07-29 16:10:29
# Description:
# 	This program is loaded as a kernel(v2.6.18 or later) module.
# 	Use "make install" to load it into kernel.
# 	Use "make remove" to remove the module out of kernel.
#
# ------------------------------------------------------------------------------


ROOT=..
#PLATFORM=$(shell $(ROOT)/systype.sh)
#include $(ROOT)/Make.defines.$(PLATFORM)

MODULE_NAME := globalmem_two
DEVICE_BASENAME := gatieme
DEVICE_COUNT := 2
IOCTL_TEST_BIN := globalmem_ioctl_test
IOCTL_TEST_SRC := globalmem_ioctl_test.c

ifneq ($(KERNELRELEASE),)

#CFG_FLAGS += -O2  -I./
#EXTRA_CFLAGS  += $(C_FLAGS) $(CFG_INC) $(CFG_INC)

obj-m := $(MODULE_NAME).o #globalmem.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2

ioctl_test: $(IOCTL_TEST_BIN)

$(IOCTL_TEST_BIN): $(IOCTL_TEST_SRC) globalmem_ioctl.h
	$(CC) $(CFLAGS) -I. -o $@ $(IOCTL_TEST_SRC)

modules:
	make -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	make -C $(KERNELDIR) M=$(PWD) modules_install



insmod: modules
	sudo insmod $(MODULE_NAME).ko globalmem_major=0
	major=$$(awk '$$2 == "globalmem" {print $$1}' /proc/devices); \
	if [ -z "$$major" ]; then \
		echo "failed to find globalmem major in /proc/devices"; \
		exit 1; \
	fi; \
	for minor in $$(seq 0 $$(($(DEVICE_COUNT) - 1))); do \
		node=/dev/$(DEVICE_BASENAME)$$minor; \
		sudo rm -f $$node; \
		sudo mknod $$node c $$major $$minor; \
		sudo chmod 666 $$node; \
	done

reinsmod:
	-$(MAKE) rmmod
	$(MAKE) insmod

github:
	cd $(ROOT) && make github

rmmod:
	-sudo rmmod $(MODULE_NAME)
	for minor in $$(seq 0 $$(($(DEVICE_COUNT) - 1))); do \
		sudo rm -f /dev/$(DEVICE_BASENAME)$$minor; \
	done

test :
	sudo ../injector/memInjector -l stack -m random -t word_0 --time 1 --timeout 3 -p 1

test_ioctl: ioctl_test
	@if [ ! -e /dev/$(DEVICE_BASENAME)0 ]; then \
		echo "/dev/$(DEVICE_BASENAME)0 not found; run 'make insmod' first."; \
		exit 1; \
	fi
	sudo ./$(IOCTL_TEST_BIN) /dev/$(DEVICE_BASENAME)0

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm -f modules.order Module.symvers Module.markers $(IOCTL_TEST_BIN)

.PHONY: modules modules_install insmod reinsmod rmmod test ioctl_test test_ioctl clean github



endif
