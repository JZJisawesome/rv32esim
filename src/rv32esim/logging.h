/* logging.h
 * By: John Jekel
 *
 * Logging facilities for rv32esim
*/

#ifndef LOGGING_H
#define LOGGING_H

/* Includes */

#include "rv32esim.h"
#include "cmake_config.h"

#include <stdarg.h>
#include <stdint.h>

/* Constants And Defines */

#if DISABLE_LOGGING
#define rvlog(...) ((void)0)
#else
#define rvlog(...) do { if (state->logging_enabled) rvlog_func(state->ninsts_executed, __VA_ARGS__); } while (0)
#endif

/* Function/Class Declarations */

void rvlog_func(uint64_t inst_num, uint8_t indent, const char* restrict str, ...);

#endif//LOGGING_H
