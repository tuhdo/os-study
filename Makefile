all: boot_loader.bin write_fat.bin boot.dsk

boot_loader.bin: boot_loader.asm
	nasm -f bin boot_loader.asm -o boot_loader.bin

write_fat.bin: write_fat.asm
	nasm -f bin write_fat.asm -o $@

boot.dsk:
	dd if=boot_loader.bin of=disk.dsk bs=512 count=2
	dd if=write_fat.bin of=disk.dsk bs=512 count=1 seek=1

clean:
	rm -rf boot_loader.bin writre_fat.bin desk.dsk
