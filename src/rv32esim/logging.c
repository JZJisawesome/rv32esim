/* logging.h
 * By: John Jekel
 *
 * Logging facilities for rv32esim
*/

/* Includes */

#include "logging.h"
#include "rv32esim.h"

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

/* Function Implementations */

__attribute__ ((visibility ("hidden"))) void rvlog_func(uint64_t inst_num, uint8_t indent, const char* restrict str, ...) {
    fprintf(stderr, "@Inst=%lu>\t", inst_num);

    while (indent--)
        fputc('\t', stderr);

    va_list list;
    assert(str);
    va_start(list, str);
    vfprintf(stderr, str, list);
    va_end(list);
    putc('\n', stderr);
}
