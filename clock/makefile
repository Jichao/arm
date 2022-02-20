CFLAGS 	:= -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -ffreestanding -std=c99
all: init.o main.o ram.o led.o int.o clock.o
	arm-linux-ld -Tint.lds -o led.elf $^ 
	arm-linux-objcopy -O binary -S led.elf led.bin
	arm-linux-objdump -D -m arm led.elf > leds.dis
%.o:%.S
	arm-linux-gcc $(CFLAGS) -c $^
%.o:%.c
	arm-linux-gcc $(CFLAGS) -c $^
clean:
	rm *.o *.elf *.bin
