/* fetch.c
 * By: John Jekel
 *
 * Code to fetch RISC-V instructions from memory
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "fetch.h"
#include "rv32esim.h"
#include "logging.h"
#include "cmake_config.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/* Function Implementations */

__attribute__ ((visibility ("hidden"))) uint32_t fetch(rv32esim_state_t* state) {//Returns 0 (invalid risc-v opcode) if anything goes wrong
    if (state->mem_len < 2) {
        rvlog(1, "ifetch failed, not enough bytes in memory to read");
        return 0;//Not enough bytes for an instruction
    }

    if ((state->pc < state->mem_begin_address) || (state->pc > (state->mem_begin_address + state->mem_len - 4))) {
        rvlog(1, "ifetch failed, PC out of bounds");
        return 0;//Out of bounds pc (need at least 32 bits in bounds for the way fetch is implemented (for simplicity of checking))
    }

#if LITTLE_ENDIAN//Little endian host makes this easy+fast :)
    if (state->pc & 0b10)
        return ((uint16_t*)state->mem)[state->pc / 2];
    else {//Can't assume instruction is 16 bits
        uint8_t inst_lsbyte = ((uint8_t*)state->mem)[state->pc];
        if ((inst_lsbyte & 0b1) && (inst_lsbyte & 0b10))//32 bit instruction
            return ((uint32_t*)state->mem)[state->pc / 4];
        else//16 bit instruction
            return ((uint16_t*)state->mem)[state->pc / 2];
    }
#else
    uint8_t* memory = (uint8_t*)state->mem;
    if (state->pc & 0b10)
        return ((uint32_t)memory[state->pc]) | (((uint32_t)memory[state->pc + 1]) << 8);
    else {//Can't assume instruction is 16 bits
        uint8_t inst_lsbyte = memory[state->pc];
        if ((inst_lsbyte & 0b1) && (inst_lsbyte & 0b10))//32 bit instruction
            return inst_lsbyte | (((uint32_t)memory[state->pc + 1]) << 8) | (((uint32_t)memory[state->pc + 2]) << 16) | (((uint32_t)memory[state->pc + 3]) << 24);
        else//16 bit instruction
            return inst_lsbyte | (((uint32_t)memory[state->pc + 1]) << 8);
    }
#endif
}
