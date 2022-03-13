/* rv32esim test program
 * By: John Jekel
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "rv32esim.h"

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
    //Initialize state
    rv32esim_state_t state;
    memset(&state, 0, sizeof(rv32esim_state_t));
    const size_t num_mem_bytes = 0xFFFFFFFFF;
    state.mem = (uint8_t*)malloc(num_mem_bytes);
    state.mem_len = num_mem_bytes;
    state.mem_w_callback8 = byte_write_callback;

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

    free(state.mem);
    return 0;
}

/* Static Function Implementations */

//TODO
