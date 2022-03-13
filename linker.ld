/* Based on JZJCore Linker Script */

/* Configure in line with the state struct */
MEMORY
{
    memory : ORIGIN = 0x00000000, LENGTH = 128M
}

/* the arch is rv64 */
OUTPUT_ARCH("riscv:rv32e")
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")

/* _start is defined in crt0.s */
ENTRY(_start)

SECTIONS
{
    PROVIDE (__executable_start = ORIGIN(memory));
    PROVIDE(__stack_top = ORIGIN(memory) + LENGTH(memory));

    .init : { *(.init*) } > memory
    .text : { *(.text*) } > memory
    .rodata : { *(.rodata*) } > memory
    .sdata :
    {
        __SDATA_BEGIN__ = .;
        *(.sdata*)
    } > memory
    .data : { *(.data*) } > memory
    .bss : { *(.bss*) } > memory
    __global_pointer$ = __SDATA_BEGIN__; /* The global pointer points to the beginning of the SDATA section */
}
