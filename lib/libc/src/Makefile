CFLAGS 	:= -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -ffreestanding -I../include
AR := arm-linux-ar 
CC := arm-linux-gcc

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
	
