all: boot_loader.bin stage2.bin kernel.bin user_space.bin boot.dsk

boot_loader.bin: boot_loader.asm
	nasm -f bin boot_loader.asm -o $@
	nasm -l boot_loader.txt boot_loader.asm

stage2.bin: stage2.asm
	nasm -f bin stage2.asm -o $@
	nasm -l stage2.txt stage2.asm

kernel.bin: kernel.asm
	nasm -f bin kernel.asm -o $@
	nasm -l kernel.txt kernel.asm

user_space.bin: user_space.asm
	nasm -f bin user_space.asm -o $@
	nasm -l user_space.txt user_space.asm

boot.dsk:
	dd if=boot_loader.bin of=disk.dsk bs=512 count=3
	dd if=stage2.bin of=disk.dsk bs=512 count=1 seek=1
	dd if=kernel.bin of=disk.dsk bs=512 count=1 seek=2
	dd if=user_space.bin of=disk.dsk bs=512 count=1 seek=3

clean:
	rm -rf *.bin disk.dsk
