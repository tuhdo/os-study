all: boot1.bin boot.dsk

boot1.bin: boot1.asm
	nasm -f bin boot1.asm -o boot1.bin

boot.dsk:
	dd if=boot1.bin of=disk.dsk bs=512 count=2
