all: boot_loader.bin boot.dsk

boot_loader.bin: boot_loader.asm
	nasm -f bin boot_loader.asm -o boot_loader.bin

boot.dsk:
	dd if=boot_loader.bin of=disk.dsk bs=512 count=2

clean:
	rm -rf boot_loader.bin desk.dsk
