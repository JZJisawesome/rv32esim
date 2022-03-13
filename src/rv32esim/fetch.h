/* fetch.h
 * By: John Jekel
 *
 * Code to fetch RISC-V instructions from memory
*/

#ifndef FETCH_H
#define FETCH_H

/* Includes */

#include "rv32esim.h"

#include <stdint.h>

/* Function/Class Declarations */

uint32_t fetch(rv32esim_state_t* state);//Returns 0 (invalid risc-v opcode) if anything goes wrong

#endif//FETCH_H
