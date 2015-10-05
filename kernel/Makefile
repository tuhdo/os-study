all: kernel.bin user_space.bin interrupt_handler.bin

%.bin: %.asm
	nasm -O1 -i$(INCLUDE_DIR) -f bin $< -o $(BUILD_DIR$)/$@
