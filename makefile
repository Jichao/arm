CC := arm-linux-gcc
OBJCOPY := arm-linux-objcopy
OBJDUMP := arm-linux-objdump

GCC_LIB_DIR := /home/book/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3

CFLAGS 	:= -nostdinc -fno-builtin -Werror -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer \
	-ffreestanding -std=c99 \
	 -Ilib/libc/include -I3rd/libmad-0.15.1b/msvc++ -Isrc

LD := arm-linux-ld
LDFLAGS := -lgcc -L$(GCC_LIB_DIR)

SOURCE_DIR := src
HEADER_DIR := src
BUILD_DIR := build
TEST_DIR := test

C_SOURCES := $(shell find $(SOURCE_DIR) -name '*.c')
ASM_SOURCES := $(shell find $(SOURCE_DIR) -name '*.S')
SOURCES := $(C_SOURCES) $(ASM_SOURCES)
C_LIB := lib/libc/src/libc.a
MP3_LIB := 3rd/libmad-0.15.1b/libmad.a

OBJECTS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
OBJECTS := $(patsubst $(SOURCE_DIR)/%.S, $(BUILD_DIR)/%.o, $(OBJECTS))

NAME := mp3
ELF := $(BUILD_DIR)/$(NAME).elf
BINARY := $(patsubst %.elf, %.bin, $(ELF))
DISASSM := $(patsubst %.elf, %.dis, $(ELF))

dir_guard=@mkdir -p $(@D)

.PHONY: all clean test setup libs

all: libs $(BINARY)

libs:
	cd lib/libc/src; make; cd ../../;
	cd 3rd/libmad-0.15.1b; make -f makefile.arm; cd ../../;

$(BINARY): $(OBJECTS) $(C_LIB) $(MP3_LIB)
	$(LD) -T$(NAME).lds -o $(ELF) $^ $(LDFLAGS)
	$(OBJCOPY) -O binary -S $(ELF) $(BINARY)
	$(OBJDUMP) -D -m arm $(ELF) > $(DISASSM)

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.S
	$(dir_guard)
	arm-linux-gcc $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.c
	$(dir_guard)
	arm-linux-gcc $(CFLAGS) -c $^ -o $@

clean:
	rm -rf build
	rm $(C_LIB)
	rm $(MP3_LIB)

test: kmalloc_test
	./kmalloc_test 

kmalloc_test: test/kmalloc_test.c kmalloc.c freelist.c bits.c common.c apple_wav.c wav.c
	gcc -DTEST -g -std=c99 $^ -o kmalloc_test
