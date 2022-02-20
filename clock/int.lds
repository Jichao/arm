SECTIONS {
    first 0x0 : {init.o main.o ram.o int.o clock.o}
    second 0x30004000 : AT(2048) { led.o }
}