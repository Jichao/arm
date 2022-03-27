CFLAGS 	:= -march=armv4t -mcpu=arm920t -Wall -Wstrict-prototypes -g -fomit-frame-pointer -ffreestanding -I../include

#AR := arm-linux-ar 
#CC := arm-linux-gcc

AR := arm-none-eabi-ar 
CC := arm-none-eabi-gcc

objs := div64.o lib1funcs.o ctype.o muldi3.o printf.o string.o vsprintf.o

libc.a: $(objs)
	${AR} -r -o $@ $^
	cp libc.a ../
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f libc.a *.o		
	
