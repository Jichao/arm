/* memory map
SDRAM 64M
+ 0x30000000
> 0~40M code memory
> 40~60M heap memory
> 60M~63M stack range
> 63M~??? vector table?(superboot) 
*/
SECTIONS {
    . = 0x30000000;
    _ram_start = .;
    .text : { *(.text) }
    .rodata ALIGN(4) : AT((LOADADDR(.text)+SIZEOF(.text)+3)&~(0x03)) {*(.rodata*)} 
    .data ALIGN(4)   : AT((LOADADDR(.rodata)+SIZEOF(.rodata)+3)&~(0x03)) { *(.data) }
    _bss_start = .;
    .bss ALIGN(4)  : { *(.bss)  *(COMMON) }
    _bss_end = .;
    _rom_size = _bss_end - _ram_start;
}