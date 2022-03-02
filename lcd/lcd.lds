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
    .text1 : {init.o(.text) main.o(.text) ram.o(.text) nand.o(.text)}

    . = 0x30004000;
    .text2 : AT(4096) { *(.text) }

    rodata_addr = (LOADADDR(.text2) + SIZEOF(.text2) + 3) & ~(0x3);
    .rodata : AT(rodata_addr) { *(.rodata*)}

    data_addr = (LOADADDR(.rodata) + SIZEOF(.rodata) + 3) & ~(0x3);
    .data : AT(data_addr) { *(.data)}

    _bss_start = .;
    .bss : {*(.bss) *(COMMON)}
    _bss_end = .;
}