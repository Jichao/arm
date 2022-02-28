SECTIONS {
    first 0x0 : {init.o main.o ram.o nand.o}
    second 0x30004000 : AT(4096) { int.o led.o clock.o entry.o timer.o uart.o rtc.o lcd.o}
}