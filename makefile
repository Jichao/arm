#CC := arm-linux-gcc
#OBJCOPY := arm-linux-objcopy
#OBJDUMP := arm-linux-objdump
#LD := arm-linux-ld

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump
LD := arm-none-eabi-ld

#GCC_LIB_DIR := /home/book/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3
GCC_LIB_DIR := /home/jcyangzh/apps/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/11.2.1

CFLAGS 	:= -march=armv4t -mcpu=arm920t -mfloat-abi=soft -mlittle-endian -nostdinc -fno-builtin -Wall -Wstrict-prototypes -g -fomit-frame-pointer \
	-ffreestanding -std=c99 \
	 -Ilib/libc/include -I3rd/libmad-0.15.1b/msvc++

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

.PHONY: all clean test setup libs

all: setup libs $(BINARY)

libs:
	cd lib/libc/src; make; cd ../../;
	cd 3rd/libmad-0.15.1b; make -f makefile.arm; cd ../../;

setup: build

build:
	mkdir -p build

$(BINARY): $(OBJECTS) $(C_LIB) $(MP3_LIB)
	$(LD) -T$(NAME).lds -o $(ELF) $^ $(LDFLAGS)
	$(OBJCOPY) -O binary -S $(ELF) $(BINARY)
	$(OBJDUMP) -D -m arm $(ELF) > $(DISASSM)

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.S
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf build
	rm $(C_LIB)
	rm $(MP3_LIB)

test: kmalloc_test
	./kmalloc_test 

kmalloc_test: test/kmalloc_test.c kmalloc.c freelist.c bits.c common.c apple_wav.c wav.c
	gcc -DTEST -g -std=c99 $^ -o kmalloc_test
