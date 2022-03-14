/* rv32esim test program
 * By: John Jekel
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "rv32esim.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

static void byte_write_callback(struct rv32esim_state_t* state, uint8_t data, uint64_t address) {
    putchar((uint8_t)data);
#ifndef NDEBUG
    fflush(stdout);
#endif
}

/* Function Implementations */

int main() {
    //Print version info
    fputs(rv32esim_version_string(), stderr);
    fputc('\n', stderr);
    fputc('\n', stderr);

    //Initialize state
    rv32esim_state_t state;
    rv32esim_init_state(&state, 0x7FFFFFFF);
    state.mem_w_callback8 = byte_write_callback;
    state.logging_enabled = true;
    state.ninsts_executed = 0;
    state.log_when_ninsts_ge = 30000000;

    //Load file for testing
    const char* filename = "test.bin";
    FILE* bin = fopen(filename, "rb");
    assert(bin);
    fseek(bin, 0 , SEEK_END);
    size_t file_size = ftell(bin);
    rewind(bin);
    assert(fread(state.mem, 1, file_size, bin) == file_size);
    fclose(bin);

    while (rv32esim_tick(&state) != ECALL);

    rv32esim_free_state(&state);
    return 0;
}

/* Static Function Implementations */

//TODO
