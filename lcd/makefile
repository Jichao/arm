CFLAGS 	:= -nostdinc -fno-builtin -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -ffreestanding -std=c99 -I../lib/include
all: init.o main.o ram.o led.o int.o clock.o nand.o entry.o timer.o uart.o rtc.o lcd.o girl.o kmalloc.o bits.o freelist.o common.o ../lib/src/libc.a
	arm-linux-ld -Tlcd.lds -o lcd.elf $^ -lgcc \
		-L/home/book/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3 
	arm-linux-objcopy -O binary -S lcd.elf lcd.bin
	arm-linux-objdump -D -m arm lcd.elf > lcd.dis
%.o:%.S
	arm-linux-gcc $(CFLAGS) -c $^
%.o:%.c
	arm-linux-gcc $(CFLAGS) -c $^
clean:
	rm *.o *.elf *.bin *.dis *_test

kmalloc_test: kmalloc_test.c kmalloc.c freelist.c bits.c common.c
	gcc -DTEST -g -std=c99 $^ -o kmalloc_test

test: kmalloc_test
	./kmalloc_test 

../lib/src/libc.a:
	cd ../lib/src; make; cd ../../rtc;
