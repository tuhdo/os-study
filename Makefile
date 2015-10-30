.DEFAULT_GOAL := all

include common.mk

all: build_bin boot.dsk

build_bin:
	[ -d build/ ] || mkdir build
	make -C bootloader
	make -C kernel

boot.dsk:
	dd if=$(BUILD_DIR)/boot_loader.bin of=disk.dsk bs=512 count=31
	dd if=$(BUILD_DIR)/stage2.bin of=disk.dsk bs=512 count=2 seek=1
	dd if=$(BUILD_DIR)/kernel.bin of=disk.dsk bs=512 count=15 seek=3
	dd if=$(BUILD_DIR)/interrupt_handler.bin of=disk.dsk bs=512 count=12 seek=18
	dd if=$(BUILD_DIR)/user_space.bin of=disk.dsk bs=512 count=1 seek=30

clean:
	rm -rf build/ disk.dsk
