all: kernel.bin user_space.bin

%.bin: %.asm
	nasm -i$(INCLUDE_DIR) -f bin $< -o $(BUILD_DIR$)/$@
