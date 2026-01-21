all: boot_loader.bin stage2.bin

%.bin: %.asm
	nasm -O1 -i$(INCLUDE_DIR) -f bin $< -o $(BUILD_DIR)/$@
