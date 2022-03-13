/* rvsim32ie
 * By: John Jekel
 *
 * RISC-V Architecture Simulator (single-hart, little endian, RV32IE)
*/

#ifndef RVSIM32IE_H
#define RVSIM32IE_H

/* Includes */

#include <stdint.h>
#include <stdbool.h>

/* Constants And Defines */

//TODO

/* Macros */

/* Types */

//TODO decide what to do with callbacks vs return codes

typedef enum {SUCCESS, CUSTOM_OPCODE, ECALL, EBREAK} rvsim_return_code_t;//TODO others/error codes/success

typedef struct rvsim_state_t {
    /* Things that are okay to modify */
    uint32_t x[15];//[0] is not stored since it is always 0
    //TODO vector registers?
    uint32_t pc;

    //Called if memory that is not mapped below is accessed
    //If null, will be interpreted the same as if returning false
    uint32_t (*mem_r_callback8)(struct rvsim_state_t* state, uint32_t address);
    void (*mem_w_callback8)(struct rvsim_state_t* state, uint8_t data, uint32_t address);
    uint32_t (*mem_r_callback16)(struct rvsim_state_t* state, uint64_t address);
    void (*mem_w_callback16)(struct rvsim_state_t* state, uint16_t data, uint32_t address);
    uint32_t (*mem_r_callback32)(struct rvsim_state_t* state, uint64_t address);
    void (*mem_w_callback32)(struct rvsim_state_t* state, uint32_t data, uint32_t address);
    void* p_flex;//Can point to/be used for whatever for callbacks to access

    //TODO callbacks for traps, or just use rvsim_return_code_t

    uint32_t mem_begin_address;//In the simulated cpu's address space
    uint32_t mem_len;//Length of memory (also of memory in simulated cpu's address space)
    uint8_t* mem;

    /* Things for rvsim internal use */
} rvsim_state_t;

/* Function Declarations */

rvsim_return_code_t rvsim_tick(rvsim_state_t* state);

#endif//RVSIM_H
