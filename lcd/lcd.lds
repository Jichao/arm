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
    .text : AT(4096) { *(.text) }
    .rodata ALIGN(4) : AT((LOADADDR(.text)+SIZEOF(.text)+3)&~(0x03)) {*(.rodata*)} 
    .data ALIGN(4)   : AT((LOADADDR(.rodata)+SIZEOF(.rodata)+3)&~(0x03)) { *(.data) }
    _bss_start = .;
    .bss ALIGN(4)  : { *(.bss)  *(COMMON) }
    _bss_end = .;
}