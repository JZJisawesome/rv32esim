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

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

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
    return result;
}

uint8_t rv32esim_version_major() {
    return VERSION_MAJOR;
}
uint8_t rv32esim_version_minor() {
    return VERSION_MINOR;
}

/* Static Function Implementations */

//TODO
