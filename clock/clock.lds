SECTIONS {
    first 0x0 : {init.o main.o ram.o nand.o}
    second 0x30004000 : AT(4096) { int.o led.o clock.o jump.o timer.o }
}