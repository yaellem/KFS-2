OS_BIN_NAME = myos.bin

OS_IMAGE_NAME = myos.img

ISO_DIR = isodir

SRCS = srcs/bootinfos.s srcs/kernel.c

OBJS_DIR = .objs

OBJS =  $(OBJS_DIR)/*

#########################################################
#                                                       #
#       Assemble the bootinfos.s containing             #
#       instructions in order to boot the kernel        #
#       correctly like multiboot header in its own      #
#       section, or reserving memory space for a stack  #
#                                                       #
#########################################################
$(OBJS_DIR): $(SRCS)
	@mkdir -p $(OBJS_DIR)
	@echo "    Creating objects file : Compiling "
	nasm -f elf32 srcs/bootinfos.s -o $(OBJS_DIR)/bootinfos.o
	gcc -m32 -ffreestanding -O2 -Wall -Wextra -std=gnu99 -c srcs/kernel.c -o $(OBJS_DIR)/kernel.o

$(OS_BIN_NAME): $(OBJS_DIR) linker/linker.ld 
	@echo "    Creating binary file : Linking"
	gcc -m32 -T linker/linker.ld -o $(OS_BIN_NAME) -ffreestanding -O2 -nolibc -nostdlib -nodefaultlibs $(OBJS)

$(OS_IMAGE_NAME): $(OS_BIN_NAME) Makefile
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(OS_BIN_NAME) $(ISO_DIR)/boot/$(OS_BIN_NAME)
	@cp grub.cfg  $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(OS_IMAGE_NAME) $(ISO_DIR) --compress=gz

bin: $(OS_BIN_NAME)

all: bin

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(OS_BIN_NAME)
	rm -rf $(ISO_DIR)

re: fclean all


multiboot: bin
	@sh test_multiboot.sh

exec: bin
	qemu-system-i386 -kernel $(OS_BIN_NAME)

exec-img: all
	qemu-system-i386 -cdrom $(OS_IMAGE_NAME)

deps:
	apt install mtools
