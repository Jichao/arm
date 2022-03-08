CC := arm-linux-gcc
OBJCOPY := arm-linux-objcopy
OBJDUMP := arm-linux-objdump

CFLAGS 	:= -nostdinc -fno-builtin -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -ffreestanding -std=c99 -I../lib/include
LD := arm-linux-ld
LDFLAGS := ../lib/src/libc.a -lgcc -L/home/book/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3 

SOURCE_DIR := src
HEADER_DIR := src
BUILD_DIR := build
TEST_DIR := test

C_SOURCES := $(shell find $(SOURCE_DIR) -name '*.c')
ASM_SOURCES := $(shell find $(SOURCE_DIR) -name '*.S')
SOURCES := $(C_SOURCES) $(ASM_SOURCES)

OBJECTS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
OBJECTS := $(patsubst $(SOURCE_DIR)/%.S, $(BUILD_DIR)/%.o, $(OBJECTS))

NAME = lcd
BINARY = lcd.bin

.PHONY: all clean test

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(LD) -Tlcd.lds -o $(NAME).elf $^ $(LDFLAGS)
	$(OBJCOPY) -O binary -S $(NAME).elf $(BINARY)
	$(OBJDUMP) -D -m arm $(NAME).elf > $(NAME).dis

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.S
	arm-linux-gcc $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.c
	arm-linux-gcc $(CFLAGS) -c $^ -o $@

clean:
	rm *.o *.elf *.bin *.dis *_test

test: kmalloc_test
	./kmalloc_test 

kmalloc_test: test/kmalloc_test.c kmalloc.c freelist.c bits.c common.c apple_wav.c wav.c
	gcc -DTEST -g -std=c99 $^ -o kmalloc_test

../lib/src/libc.a:
	cd ../lib/src; make; cd ../../rtc;
