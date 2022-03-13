/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Constants And Defines */

//TODO

/* Includes */

#include "logging.h"
#include "rv32esim.h"

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

/* Types */

//TODO

/* Variables */

//TODO

/* Static Function Declarations */

//TODO

/* Function Implementations */

__attribute__((visibility ("hidden"))) void rvlog(const rv32esim_state_t* restrict state, uint8_t indent, const char* restrict str, ...) {
    assert(state);
    if (!state->logging_enabled)
        return;

    while (indent--)
        fputc('\t', stderr);

    va_list list;
    assert(str);
    va_start(list, str);
    vfprintf(stderr, str, list);
    va_end(list);
}

/* Static Function Implementations */

//TODO
