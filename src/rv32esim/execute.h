/* execute.h
 * By: John Jekel
 *
 * Code to execute decoded RISC-V instructions
*/

#ifndef EXECUTE_H
#define EXECUTE_H

/* Includes */

#include "rv32esim.h"

#include "decode.h"

/* Constants And Defines */

//TODO

/* Types */

//TODO

/* Global Variables */

//TODO

/* Function/Class Declarations */

rv32esim_return_code_t execute(rv32esim_state_t* state, const decoded_inst_t* decoded_inst);

#endif//EXECUTE_H
