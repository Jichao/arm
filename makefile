CC := arm-linux-gcc
OBJCOPY := arm-linux-objcopy
OBJDUMP := arm-linux-objdump

GCC_LIB_DIR := /home/book/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3
CFLAGS 	:= -nostdinc -fno-builtin -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -ffreestanding -std=c99 -Ilib/libc/include
LD := arm-linux-ld
LDFLAGS := -lgcc -L$(GCC_LIB_DIR)

SOURCE_DIR := src
HEADER_DIR := src
BUILD_DIR := build
TEST_DIR := test

C_SOURCES := $(shell find $(SOURCE_DIR) -name '*.c')
ASM_SOURCES := $(shell find $(SOURCE_DIR) -name '*.S')
SOURCES := $(C_SOURCES) $(ASM_SOURCES)
C_LIB := lib/libc/libc.a
MP3_LIB := 3rd/libmad

OBJECTS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
OBJECTS := $(patsubst $(SOURCE_DIR)/%.S, $(BUILD_DIR)/%.o, $(OBJECTS))

NAME := mp3
ELF := $(BUILD_DIR)/$(NAME).elf
BINARY := $(patsubst %.elf, %.bin, $(ELF))
DISASSM := $(patsubst %.elf, %.dis, $(ELF))

.PHONY: all clean test setup

all: setup $(BINARY)

setup: build

build:
	mkdir -p build

$(BINARY): $(OBJECTS) $(C_LIB)
	$(LD) -T$(NAME).lds -o $(ELF) $^ $(LDFLAGS)
	$(OBJCOPY) -O binary -S $(ELF) $(BINARY)
	$(OBJDUMP) -D -m arm $(ELF) > $(DISASSM)

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.S
	arm-linux-gcc $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.c
	arm-linux-gcc $(CFLAGS) -c $^ -o $@

$(C_LIB):
	cd lib/libc/src; make; cd ../../;

clean:
	rm -rf build

test: kmalloc_test
	./kmalloc_test 

kmalloc_test: test/kmalloc_test.c kmalloc.c freelist.c bits.c common.c apple_wav.c wav.c
	gcc -DTEST -g -std=c99 $^ -o kmalloc_test
