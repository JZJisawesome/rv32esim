/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

/* Types */

//TODO

/* Variables */

/* Static Function Declarations */

static void print_string(const char* str);
static void print_uint(uint64_t uint);
static void print_uint_bin(uint64_t uint);
//static void rvsim_printf(const char* str, ...);

/* Function Implementations */

int main() {
    //printf("Testing");
    print_string("Hello world! C running on RISC-V!\n");
    //print_string("\xAA\xBB\xCC\xDD\xEE\xFF");
    volatile uint64_t a = 1;
    volatile uint64_t b = 2;

    assert((a + b) == 3);
    assert((a - b) == 0xFFFFFFFFFFFFFFFF);
    assert((a << b) == 4);
    assert((a >> b) == 0);
    assert(a < b);
    volatile int64_t ai = 0x8000000000000000;
    volatile uint64_t bi = 3;
    print_uint_bin(ai >> bi);
    print_string("\n");
    assert((ai >> bi) == 0xF000000000000000);

    assert((b * bi) == 6);
    assert(false && "Sanity check assert works");
}

void __assert_func(const char* file, int line, const char* function, const char* expr) {
    print_string("!!!Assertion failed: ");
    print_string(file);
    print_string(" | ");
    print_string(function);
    print_string(" | ");
    print_string(expr);
    print_string("\n");
    __asm__ volatile ("ecall");
}

/* Static Function Implementations */

static void print_string(const char* str) {
    while (*str)
        *((volatile uint8_t*)-1) = *(str++);
}

static void print_uint(uint64_t uint) {//TODO do this more efficiently
    char buffer[32];
    buffer[31] = 0x00;//Null terminator

    uint64_t index = 30;
    while (uint) {
        buffer[index] = '0' + (uint % 10);

        --index;
        uint /= 10;
    }
    print_string(&buffer[index + 1]);
}

static void print_uint_bin(uint64_t uint) {
    *((volatile uint8_t*)-1) = '0';
    *((volatile uint8_t*)-1) = 'b';

    uint64_t mask = 1ul << 63;
    bool first_one_encountered = false;
    while (mask) {

        if (uint & mask) {
            *((volatile uint8_t*)-1) = '1';
            first_one_encountered = true;
        } else if (first_one_encountered)
            *((volatile uint8_t*)-1) = '0';

        mask >>= 1;
    }

    if (!first_one_encountered)
            *((volatile uint8_t*)-1) = '0';
}
