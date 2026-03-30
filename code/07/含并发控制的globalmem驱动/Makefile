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


#	my driver description
DRIVER_VERSION := "1.0.0"
DRIVER_AUTHOR  := "Gatieme @ AderStep Inc..."
DRIVER_DESC    := "Linux input module for Elo MultiTouch(MT) devices"
DRIVER_LICENSE := "Dual BSD/GPL"


MODULE_NAME := globalmem_lock
DEVICE_BASENAME := gatieme
DEVICE_COUNT := 1



ifneq ($(KERNELRELEASE),)
	obj-m := $(MODULE_NAME).o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

LINUX_KERNEL ?= $(shell uname -r)
LINUX_KERNEL_PATH ?= /lib/modules/$(LINUX_KERNEL)/build

CURRENT_PATH ?= $(shell pwd)
CFG_INC = $(CURRENT_PATH)
MODCFLAGS:=-O2 -Wall -DMODULE -D__KERNEL__ -DLINUX -std=c99
EXTRA_CFLAGS  += $(MODULE_FLAGS) -I $(CFG_INC)


modules:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

modules_install:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules_install

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

rmmod:
	sudo rmmod $(MODULE_NAME)
	for minor in $$(seq 0 $$(($(DEVICE_COUNT) - 1))); do \
		sudo rm -f /dev/$(DEVICE_BASENAME)$$minor; \
	done

github:
	cd $(ROOT) && make github

test: reinsmod
	echo "gatieme" > /dev/$(DEVICE_BASENAME)0
	cat /dev/$(DEVICE_BASENAME)0

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
	rm -f modules.order Module.symvers Module.markers


.PHONY: 
	modules modules_install insmod reinsmod rmmod test clean github



endif
