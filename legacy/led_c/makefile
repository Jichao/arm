CFLAGS 	:= -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -ffreestanding
all: crt.o main.o
	arm-linux-ld -Ttext 0x0 -o led.elf $^
	arm-linux-objcopy -O binary -S led.elf led.bin
	arm-linux-objdump -D -m arm led.elf > leds.dis
%.o:%.S
	arm-linux-gcc $(CFLAGS) -c $^
%.o:%.c
	arm-linux-gcc $(CFLAGS) -c $^
clean:
	rm *.o *.elf *.bin
