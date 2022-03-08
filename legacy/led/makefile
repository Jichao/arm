all: led.o
	arm-linux-ld -Ttext 0x0 -o led.elf $^
	arm-linux-objcopy -O binary -S led.elf led.bin
%.o:%.S
	arm-linux-gcc -g -c $^
%.o:%.c
	arm-linux-gcc -g -c $^
clean:
	rm *.o *.elf *.bin
