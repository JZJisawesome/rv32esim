#Based on JZJCore implementation

.section .init, "ax"
.global _start
_start:
    #Hint to assembler about start of function
    .cfi_startproc
    .cfi_undefined ra

    #Setup global pointer
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    #Setup stack pointer based on linker script symbol
    la sp, __stack_top
    #Also set s0 to the value in the stack pointer
    #addi s0, sp, 0

    #Jump to main
    jal x1, main

    #Halt cpu if we ever return from main
    #TODO we should be calling destructors (the c atexit() function) before we halt
    ecall

    #Hint to the assembler about the end of the function
    .cfi_endproc

    #Hint to assembler about end of file
    .end
