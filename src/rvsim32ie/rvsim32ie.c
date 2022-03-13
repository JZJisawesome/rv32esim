/* rvsim32ie
 * By: John Jekel
 *
 * RISC-V Architecture Simulator (single-hart, little endian, RV32ICE)
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "rvsim32ie.h"
//#include "fetch.h"
//#include "decode.h"
//#include "execute.h"
//#include "debug_logging.h"

#include <stdbool.h>
#include <stdint.h>

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

rvsim32ie_return_code_t rvsim32ie_tick(rvsim32ie_state_t* state) {
    /*rvsim_debug_log(0, "!----- BEGIN TICK: PC = 0x%llX ----------------------!\n", state->pc);

    uint32_t instruction = rvsim_fetch(state);
    rvsim_debug_log(1, "instruction = 0x%llX\n", instruction);

    decoded_inst_t decoded_inst;
    rvsim_decode(&decoded_inst, instruction);

    rvsim_return_code_t result = rvsim_execute(state, &decoded_inst);
    rvsim_debug_log(0, "!----- END TICK:   PC = 0x%llX | result = 0x%llX -----!\n\n", state->pc, (uint64_t)result);
    return result;*/
}

/* Static Function Implementations */

//TODO
