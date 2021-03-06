/* rv32esim
 * By: John Jekel
 *
 * RISC-V Architecture Simulator (single-hart, little endian, RV32ICE)
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "rv32esim.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "logging.h"
#include "cmake_config.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

void rv32esim_init_state(rv32esim_state_t* state, uint32_t mem_len) {
    assert(state);
    assert(mem_len);
    //Set PC to 0x00000000, ninsts_executed to 0, logging off, and callbacks to NULL all at once
    memset(state, 0, sizeof(rv32esim_state_t));
    state->mem_len = mem_len;
    state->mem = malloc(mem_len);
    assert(state->mem);
}

void rv32esim_free_state(rv32esim_state_t* state) {
    assert(state);
    assert(state->mem);
    free(state->mem);
}

rv32esim_return_code_t rv32esim_tick(rv32esim_state_t* state) {
    rvlog(0, " PC = 0x%llX | Tick Begins", state->pc);

    //Fetch
    uint32_t instruction = fetch(state);
    rvlog(1, "instruction = 0x%llX", instruction);

    //Decode
    decoded_inst_t decoded_inst;
    decode(state, &decoded_inst, instruction);

    //Execute
    rv32esim_return_code_t result = execute(state, &decoded_inst);
    rvlog(0, " PC = 0x%llX | result = 0x%llX | Tick Ends", state->pc, (uint32_t)result);

    ++(state->ninsts_executed);
    return result;
}

uint8_t rv32esim_version_major() {
    return VERSION_MAJOR;
}

uint8_t rv32esim_version_minor() {
    return VERSION_MINOR;
}

const char* rv32esim_version_string() {
    return VERSION_STRING;
}

/* Static Function Implementations */

//TODO
