PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
MOD_DIR := /lib/modules/$(shell uname -r)/kernel/driver/misc
DRV_NAME := ex_bin_tree
SEC_DRV_NAME := ex_bin_search
obj-m := $(DRV_NAME).o
obj-m += $(SEC_DRV_NAME).o
CC := x86_64-linux-gnu-gcc

.PHONY: build run remove install uninstall clean check format

build:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

run: build
	insmod $(PWD)/$(DRV_NAME).ko
	insmod $(PWD)/$(SEC_DRV_NAME).ko

remove:
	rmmod $(DRV_NAME)
	rmmod $(SEC_DRV_NAME)

install: build
	@echo "Установка модуля..."
	@sudo cp $(PWD)/$(DRV_NAME).ko $(MOD_DIR)
	@sudo cp $(PWD)/$(SEC_DRV_NAME).ko $(MOD_DIR)
	@sudo depmod -a
	@sudo insmod $(MOD_DIR)$(DRV_NAME).ko
	@sudo insmod $(MOD_DIR)$(SEC_DRV_NAME).ko

uninstall:
	@echo "Удаление модуля..."
	@sudo modprobe -r $(DRV_NAME)
	@sudo modprobe -r $(SEC_DRV_NAME)
	@sudo rm -f $(MOD_DIR)$(DRV_NAME).ko
	@sudo rm -f $(MOD_DIR)$(SEC_DRV_NAME).ko
	@sudo depmod -a

clean: 
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

format:
	clang-format -i $(DRV_NAME).c
	clang-format -i $(SEC_DRV_NAME).c

check: build run remove
	dmesg | tail -n 40

all: build
