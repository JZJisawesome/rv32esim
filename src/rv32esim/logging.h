/* common.h
 * By: John Jekel
 *
 * Logging facilities for rv32esim
*/

#ifndef LOGGING_H//TODO
#define LOGGING_H//TODO

/* Includes */

#include "rv32esim.h"

#include <stdarg.h>
#include <stdint.h>

/* Function/Class Declarations */

__attribute__((visibility ("hidden"))) void rvlog(const rv32esim_state_t* restrict state, uint8_t indent, const char* restrict str, ...);

#endif//LOGGING_H
