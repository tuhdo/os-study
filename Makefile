all: boot_loader.bin kernel.bin boot.dsk

boot_loader.bin: boot_loader.asm
	nasm -f bin boot_loader.asm -o boot_loader.bin

kernel.bin: kernel.asm
	nasm -f bin kernel.asm -o $@

boot.dsk:
	dd if=boot_loader.bin of=disk.dsk bs=512 count=2
	dd if=kernel.bin of=disk.dsk bs=512 count=1 seek=1

clean:
	rm -rf *.bin disk.dsk
