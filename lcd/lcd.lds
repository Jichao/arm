/* memory map
SDRAM 64M
+ 0x30000000
> 1~16K memory section table
> 16K~40M code range
~ 40M~60M heap range
> 60M~x bss range
> x~64M stack range
*/
SECTIONS {
    . = 0x0;
    first : {init.o main.o ram.o nand.o}
    . = 0x30004000;
    second : AT(4096) { int.o led.o clock.o entry.o timer.o uart.o rtc.o lcd.o}
    . = 0x33c00000;
    .bss : {*(.bss)}
}