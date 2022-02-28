CFLAGS 	:= -nostdinc -fno-builtin -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -ffreestanding -std=c99 -I../lib/include
all: init.o main.o ram.o led.o int.o clock.o nand.o entry.o timer.o uart.o rtc.o ../lib/src/libc.a
	arm-linux-ld -Tlcd.lds -o led.elf $^ -lgcc \
		-L/home/book/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3 
	arm-linux-objcopy -O binary -S led.elf led.bin
	arm-linux-objdump -D -m arm led.elf > leds.dis
%.o:%.S
	arm-linux-gcc $(CFLAGS) -c $^
%.o:%.c
	arm-linux-gcc $(CFLAGS) -c $^
clean:
	rm *.o *.elf *.bin

../lib/src/libc.a:
	cd ../lib/src; make; cd ../../rtc;
